import "./reset.css";
import dayjs from "dayjs";
import content from "../content.vue";
import { message } from "@/utils/message";
import { addDialog } from "@/components/ReDialog";
import type { PaginationProps } from "@pureadmin/table";
import { getKeyList, deviceDetection } from "@pureadmin/utils";
import {
  getShortSuggestionsList,
  deleteShortSuggestion
} from "@/api/suggestions";
import { type Ref, h, ref, toRaw, computed, reactive, onMounted } from "vue";

import * as XLSX from "xlsx";

export function useSuggestions(tableRef: Ref) {
  const form = reactive({
    title: "",
    type: -1,
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
      label: "勾选列", // 如果需要表格多选，此处label必须设置
      type: "selection",
      fixed: "left",
      reserveSelection: true // 数据刷新后保留选项
    },
    {
      label: "ID",
      prop: "id",
      width: 90
    },
    {
      label: "标题",
      slot: "title",
      width: 130
    },
    {
      label: "AI评分",
      slot: "score",
      minWidth: 90
    },
    {
      label: "赞同",
      prop: "votes",
      minWidth: 90
    },
    {
      label: "公开",
      prop: "isPublic",
      minWidth: 90,
      formatter: ({ isPublic }) => (isPublic ? "是" : "否")
    },
    {
      label: "匿名",
      prop: "isAnonymous",
      minWidth: 90,
      formatter: ({ isAnonymous }) => (isAnonymous ? "是" : "否")
    },
    {
      label: "类型",
      slot: "type",
      minWidth: 90
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

  async function handleDelete(row) {
    const { success } = await deleteShortSuggestion({ ids: [row.id] });
    if (success) {
      message(`删除成功`, { type: "success" });
    } else {
      message(`删除失败`, { type: "error" });
      return;
    }
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

  /** 当CheckBox选择项发生变化时会触发该事件 */
  function handleSelectionChange(val) {
    selectedNum.value = val.length;
    // 重置表格高度
    tableRef.value.setAdaptive();
  }

  /** 取消选择 */
  function onSelectionCancel() {
    selectedNum.value = 0;
    // 用于多选表格，清空的选择
    tableRef.value.getTableRef().clearSelection();
  }

  /** 批量删除 */
  async function onbatchDel() {
    // 返回当前选中的行
    const curSelected = tableRef.value.getTableRef().getSelectionRows();
    const { success } = await deleteShortSuggestion({
      ids: getKeyList(curSelected, "id")
    });
    if (success) {
      message(`已删除ID为 ${getKeyList(curSelected, "id")} 的数据`, {
        type: "success"
      });
      tableRef.value.getTableRef().clearSelection();
    } else {
      message(`删除失败`, { type: "error" });
    }
    onSearch();
  }

  async function onSearch() {
    loading.value = true;
    const { success, data } = await getShortSuggestionsList({
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
      width: "40%",
      top: "10vh",
      draggable: true,
      fullscreen: deviceDetection(),
      fullscreenIcon: true,
      closeOnClickModal: false,
      hideFooter: true,
      props: {
        row: row
      },
      contentRenderer: () => h(content, { ...row })
    });
  }

  function exportCurrent() {
    const header = [
      { key: "id", label: "ID" },
      { key: "title", label: "标题" },
      { key: "score", label: "AI评分" },
      { key: "votes", label: "赞同" },
      { key: "isPublic", label: "公开" },
      { key: "isAnonymous", label: "匿名" },
      { key: "type", label: "类型" },
      { key: "content", label: "内容" },
      { key: "name", label: "姓名" },
      { key: "class", label: "班级" },
      { key: "grade", label: "年级" },
      { key: "date", label: "日期" }
    ];

    const typeMap = {
      0: "饭堂建议",
      1: "教学建议",
      2: "环境建议",
      3: "其他建议"
    };
    const boolMap = {
      false: "否",
      true: "是"
    };

    const exportData = dataList.value.map(item => ({
      ID: item.id,
      标题: item.title,
      AI评分: item.score,
      赞同: item.votes,
      公开: boolMap[item.isPublic],
      匿名: boolMap[item.isAnonymous],
      类型: typeMap[item.type],
      内容: item.content,
      姓名: item.name,
      班级: item.class,
      年级: item.grade,
      日期: dayjs(item.date).format("YYYY-MM-DD")
    }));

    const ws = XLSX.utils.json_to_sheet(exportData, {
      header: header.map(h => h.label)
    });
    const wb = XLSX.utils.book_new();
    XLSX.utils.book_append_sheet(wb, ws, "短建议列表");
    XLSX.writeFile(wb, "短建议列表.xlsx");
  }

  return {
    form,
    loading,
    columns,
    dataList,
    pagination,
    selectedNum,
    buttonClass,
    deviceDetection,
    onSearch,
    resetForm,
    onbatchDel,
    onSelectionCancel,
    handleDelete,
    handleSizeChange,
    handleCurrentChange,
    handleSelectionChange,
    openDetailDialog,
    exportCurrent,
    sortByScoreAsc,
    sortByScoreDesc
  };
}
