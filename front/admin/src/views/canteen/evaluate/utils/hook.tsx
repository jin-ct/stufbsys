import "./reset.css";
import dayjs from "dayjs";
import detail from "../detail.vue";
import { message } from "@/utils/message";
import { addDialog } from "@/components/ReDialog";
import type { PaginationProps } from "@pureadmin/table";
import { deviceDetection } from "@pureadmin/utils";
import { getDishList, exportAllEvaluates } from "@/api/canteen";
import { type Ref, h, ref, toRaw, computed, reactive, onMounted } from "vue";
import * as XLSX from "xlsx";

export function useDish(tableRef: Ref) {
  const form = reactive({
    dishname: "",
    date: ["", ""]
  });
  const sortByScore = ref("");
  const dataList = ref([]);
  const loading = ref(true);
  const selectedNum = ref(0);
  const pagination = reactive<PaginationProps>({
    total: 0,
    pageSize: 10,
    currentPage: 1,
    background: true
  });
  const columns: TableColumnList = [
    {
      label: "ID",
      prop: "dish_id",
      width: 90
    },
    {
      label: "菜品名称",
      prop: "name",
      width: 90
    },
    {
      label: "平均分数",
      slot: "score",
      minWidth: 90
    },
    {
      label: "评价总人数",
      prop: "evaluate_count",
      minWidth: 90
    },
    {
      label: "改进建议（最新）",
      slot: "suggestion",
      minWidth: 130
    },
    {
      label: "日期",
      minWidth: 100,
      prop: "date",
      formatter: ({ date }) => dayjs(date).format("YYYY-MM-DD")
    },
    {
      label: "操作",
      fixed: "right",
      width: 180,
      slot: "operation"
    }
  ];
  const buttonClass = computed(() => {
    return [
      "h-[20px]!",
      "reset-margin",
      "text-gray-500!",
      "dark:text-white!",
      "dark:hover:text-primary!"
    ];
  });

  function handleSizeChange(val: number) {
    // console.log(`${val} items per page`);
    pagination.pageSize = val;
    pagination.currentPage = 1;
    onSearch();
  }

  function handleCurrentChange(val: number) {
    // console.log(`current page: ${val}`);
    pagination.currentPage = val;
    onSearch();
  }

  function sortByScoreAsc() {
    sortByScore.value = "asc";
    onSearch();
  }

  function sortByScoreDesc() {
    sortByScore.value = "desc";
    onSearch();
  }

  /** 当CheckBox选择项发生变化时会触发该事件 */
  function handleSelectionChange(val) {
    selectedNum.value = val.length;
    // 重置表格高度
    tableRef.value.setAdaptive();
  }

  async function onSearch() {
    loading.value = true;
    const { success, data } = await getDishList({
      search: toRaw(form),
      pageSize: pagination.pageSize,
      toPage: pagination.currentPage,
      sortByScore: sortByScore.value
    });

    if (success) {
      dataList.value = data.list;
      pagination.total = data.total;
    }

    setTimeout(() => {
      loading.value = false;
    }, 500);
  }

  const resetForm = formEl => {
    if (!formEl) return;
    formEl.resetFields();
    sortByScore.value = "";
    onSearch();
  };

  onMounted(async () => {
    onSearch();
  });

  function openDetailDialog(title = "详情", row) {
    addDialog({
      title: `${title}`,
      width: "74%",
      top: "5vh",
      draggable: true,
      fullscreen: deviceDetection(),
      fullscreenIcon: true,
      closeOnClickModal: false,
      hideFooter: true,
      props: {
        id: row?.dish_id,
        name: row?.name
      },
      contentRenderer: () => h(detail, { id: row?.dish_id, name: row?.name })
    });
  }

  async function exportAll() {
    const { success, data } = await exportAllEvaluates();
    if (!success) {
      message("导出失败", { type: "error" });
      return;
    }
    handleExportData(data);
  }

  function handleExportData(data) {
    const wb = XLSX.utils.book_new();
    let sheetData: any[] = [];
    data.forEach((item, idx) => {
      // 大表头
      sheetData.push([
        `【${item.name}】 平均分数: ${parseFloat(Number(item.avg_score).toFixed(2))}  评价总数: ${item.evaluate_count}  日期: ${item.date}`
      ]);
      // evaluates表头
      // 将“建议”这一列拉长
      sheetData.push([
        "ID",
        "分数",
        "赞同数",
        { v: "建议", s: { width: 40 } }, // 设置建议列宽
        "日期"
      ]);
      // evaluates数据
      item.evaluates?.forEach(ev => {
        sheetData.push([
          ev.id,
          ev.score,
          ev.vote_count,
          ev.suggestion,
          ev.date
        ]);
      });
      // 空行分隔
      if (idx !== data.length - 1) sheetData.push([]);
    });

    const ws = XLSX.utils.aoa_to_sheet(sheetData);
    // 合并大表头单元格
    let rowIdx = 0;
    data.forEach((item, idx) => {
      ws["!merges"] = ws["!merges"] || [];
      ws["!merges"].push({
        s: { r: rowIdx, c: 0 },
        e: { r: rowIdx, c: 4 }
      });
      // 跳过大表头+表头+数据+空行
      rowIdx += 2 + (item.evaluates?.length || 0) + 1;
    });

    XLSX.utils.book_append_sheet(wb, ws, "菜品评价");
    XLSX.writeFile(wb, "菜品评价.xlsx");
  }

  return {
    form,
    loading,
    columns,
    dataList,
    pagination,
    buttonClass,
    deviceDetection,
    onSearch,
    resetForm,
    handleSizeChange,
    handleCurrentChange,
    handleSelectionChange,
    openDetailDialog,
    exportAll,
    sortByScoreAsc,
    sortByScoreDesc
  };
}
