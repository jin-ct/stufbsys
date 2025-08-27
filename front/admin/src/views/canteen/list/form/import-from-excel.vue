<script setup lang="ts">
import { ref } from "vue";
import * as XLSX from "xlsx/xlsx.mjs";
import ReCol from "@/components/ReCol";
import { message } from "@/utils/message";

const props = defineProps<{
  formInline: {
    title: string;
    isCover: boolean;
    isReady: boolean;
    excelName: string;
    data: object[];
  };
}>();

const newFormInline = ref(props.formInline);

const excelName = ref("");
const tableData = ref([]);
const tableHeader = ref([]);

function readFile(file) {
  //文件读取
  return new Promise(resolve => {
    let reader = new FileReader();
    reader.readAsArrayBuffer(file);
    reader.onload = ev => {
      const arrayBuffer = ev.target.result as ArrayBuffer;
      const binaryString = String.fromCharCode(...new Uint8Array(arrayBuffer));
      resolve(binaryString);
    };
  });
}

function getHeader(sheet) {
  // const XLSX = XLSX;
  const headers = [];
  const range = XLSX.utils.decode_range(sheet["!ref"]); // worksheet['!ref'] 是工作表的有效范围
  let C;
  /* 获取单元格值 start in the first row */

  const R = range.s.r; // 行 // C 列
  let i = 0;
  // s:开始start，e:结束end
  for (C = range.s.c; C <= range.e.c; ++C) {
    var cell =
      sheet[
        XLSX.utils.encode_cell({ c: C, r: R })
      ]; /* 根据地址得到单元格的值find the cell in the first row */
    var hdr = "UNKNOWN" + C; // 如果有空表头，会替换为您想要的默认值replace with your desired default
    // XLSX.utils.format_cell 生成单元格文本值
    if (cell && cell.t) hdr = XLSX.utils.format_cell(cell);
    if (hdr.indexOf("UNKNOWN") > -1) {
      if (!i) {
        hdr = "__EMPTY";
      } else {
        hdr = "__EMPTY_" + i;
      }
      i++;
    }
    headers.push(hdr);
  }
  // 保存至data中
  tableHeader.value = headers;

  return headers;
}

async function handle(ev) {
  let file = ev.raw;
  excelName.value = file.name;
  //截取表格文件名
  excelName.value = excelName.value.substring(
    0,
    excelName.value.lastIndexOf(".")
  );
  if (!file) {
    message(`您未选择文件`, {
      type: "warning"
    });
    return;
  } else {
    let data = await readFile(file);
    let workbook = XLSX.read(data, { type: "binary" }); //解析二进制格式数据
    let worksheet = workbook.Sheets[workbook.SheetNames[0]]; //获取第一个Sheet
    // 调用解析表头方法
    getHeader(worksheet);
    let result = XLSX.utils.sheet_to_json(worksheet); //转换为json数据格式
    // 表格数据
    tableData.value = result;
    // console.log('表格头tableHeader：', tableHeader.value);
    // console.log('表格数据tableData：', tableData.value);
    if (
      !tableHeader.value.includes("名称") ||
      !tableHeader.value.includes("日期")
    ) {
      message(`必须包含“名称”和“日期”字段`, {
        type: "warning"
      });
    } else {
      // 格式化“日期”列为“YYYY-MM-DD”
      tableData.value.forEach(row => {
        if (row["日期"]) {
          // Excel日期可能是字符串或数字（序列号）
          let dateValue = row["日期"];
          let date;
          if (typeof dateValue === "number") {
            // Excel序列号转日期
            date = XLSX.SSF.parse_date_code(dateValue);
            if (date) {
              const pad = n => (n < 10 ? "0" + n : n);
              row["日期"] = `${date.y}-${pad(date.m)}-${pad(date.d)}`;
            }
          } else if (typeof dateValue === "string") {
            // 兼容字符串日期
            date = new Date(dateValue.replace(/-/g, "/"));
            if (!isNaN(date.getTime())) {
              const pad = n => (n < 10 ? "0" + n : n);
              row["日期"] =
                `${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())}`;
            }
          }
        }
      });
      newFormInline.value.data = tableData.value.map(row => {
        return {
          name: row["名称"],
          description: row["描述"],
          date: row["日期"]
        };
      });
      // console.log('解析后的数据newFormInline.value.data：', newFormInline.value.data);
      newFormInline.value.isReady = true;
      message(`表格解析成功`, {
        type: "success"
      });
    }
  }
}
</script>

<template>
  <el-form :model="newFormInline" label-width="82px">
    <el-row :gutter="30">
      <re-col :value="12" :xs="24" :sm="24">
        <el-form-item label="导入方式" prop="importMethod">
          <el-select
            v-model="newFormInline.isCover"
            placeholder="请选择导入方式"
          >
            <el-option label="覆盖现有数据" :value="true" />
            <el-option label="追加到现有数据" :value="false" />
          </el-select>
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="导入" prop="import">
          <el-upload
            accept=".xlsx, .xls"
            :auto-upload="false"
            :show-file-list="false"
            :on-change="handle"
          >
            <el-button type="primary" class="imExcelBtn">导入Excel</el-button>
          </el-upload>
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="提示">
          <el-text>
            请确保导入的Excel文件包含必要的字段，并在对应数据列上注明表头<br />
            必要字段包括：名称、日期<br />
            <span style="color: red"
              >请使用标准表头文字（名称、描述、日期），其他表头文字可能无法识别</span
            >
          </el-text>
        </el-form-item>
      </re-col>
    </el-row>
  </el-form>
</template>

<style scoped>
.tips {
  margin-bottom: 20px;
  color: #999;
  font-size: 14px;
}
.tips p {
  margin: 0;
}
</style>
