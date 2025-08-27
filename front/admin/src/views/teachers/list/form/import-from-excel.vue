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
  const headers = [];
  const range = XLSX.utils.decode_range(sheet["!ref"]); // worksheet['!ref'] 是工作表的有效范围
  let C;

  const R = range.s.r; // 行 // C 列
  let i = 0;
  // s:开始start，e:结束end
  for (C = range.s.c; C <= range.e.c; ++C) {
    var cell = sheet[XLSX.utils.encode_cell({ c: C, r: R })];
    var hdr = "UNKNOWN" + C;
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
      !tableHeader.value.includes("姓名") ||
      !tableHeader.value.includes("科目") ||
      !tableHeader.value.includes("年级") ||
      !tableHeader.value.includes("班级")
    ) {
      message(`必须包含“姓名”、“科目”、“年级”和“班级”字段`, {
        type: "warning"
      });
    } else {
      newFormInline.value.data = tableData.value.map(row => {
        return {
          name: row["姓名"],
          subject: row["科目"],
          grade: row["年级"],
          class: row["班级"],
          email: row["邮箱"]
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
            必要字段包括：姓名、科目、年级、<span style="color: black"
              >班级（多个用“/”隔开）</span
            ><br />
            <span style="color: red"
              >请使用标准表头文字（姓名、科目、年级、班级、邮箱），其他表头文字可能无法识别</span
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
