import "./reset.css";
import dayjs from "dayjs";
import { message } from "@/utils/message";
import type { PaginationProps } from "@pureadmin/table";
import { getKeyList, deviceDetection } from "@pureadmin/utils";
import { getDishAddList, deleteDishAdd, approveDishAdd } from "@/api/canteen";
import { type Ref, ref, toRaw, computed, reactive, onMounted } from "vue";

import * as XLSX from "xlsx";

export function useAddDish(tableRef: Ref) {
  const form = reactive({
    dishname: "",
    date: ["", ""]
  });
  const sortByScore = ref("");
  const sortByVote = ref("");
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
      label: "菜品名称",
      prop: "name",
      minWidth: 90
    },
    {
      label: "AI评分",
      slot: "score",
      minWidth: 90
    },
    {
      label: "票数",
      prop: "vote",
      minWidth: 80
    },
    {
      label: "描述",
      slot: "description",
      minWidth: 130
    },
    {
      label: "状态",
      slot: "status",
      width: 90
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

  function handleUpdate(row) {
    console.log(row);
  }

  async function handleDelete(row) {
    const { success } = await deleteDishAdd({ ids: [row.id] });
    if (success) {
      message(`删除成功`, { type: "success" });
    } else {
      message(`删除失败`, { type: "error" });
      return;
    }
    onSearch();
  }

  async function approve(row) {
    const { success } = await approveDishAdd({ id: row.id });
    if (success) {
      message(`操作成功`, { type: "success" });
    } else {
      message(`操作失败`, { type: "error" });
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

  function sortByVotesAsc() {
    sortByVote.value = "asc";
    onSearch();
  }

  function sortByVotesDesc() {
    sortByVote.value = "desc";
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
    const { success } = await deleteDishAdd({
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
    const { success, data } = await getDishAddList({
      search: toRaw(form),
      pageSize: pagination.pageSize,
      toPage: pagination.currentPage,
      sortByScore: sortByScore.value,
      sortByVote: sortByVote.value
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
    sortByVote.value = "";
    onSearch();
  };

  onMounted(async () => {
    onSearch();
  });

  function exportCurrent() {
    const header = [
      { key: "id", label: "ID" },
      { key: "name", label: "菜品名称" },
      { key: "description", label: "描述" },
      { key: "vote", label: "票数" },
      { key: "state", label: "状态" },
      { key: "score", label: "AI评分" },
      { key: "date", label: "日期" }
    ];

    const stateMap = {
      0: "已申请",
      1: "公开投票"
    };

    const exportData = dataList.value.map(item => ({
      ID: item.id,
      菜品名称: item.name,
      描述: item.description,
      票数: item.vote,
      状态: stateMap[item.states] ?? item.states,
      AI评分: item.score,
      日期: dayjs(item.date).format("YYYY-MM-DD")
    }));

    const ws = XLSX.utils.json_to_sheet(exportData, {
      header: header.map(h => h.label)
    });
    const wb = XLSX.utils.book_new();
    XLSX.utils.book_append_sheet(wb, ws, "加菜列表");
    XLSX.writeFile(wb, "加菜列表.xlsx");
  }

  return {
    form,
    loading,
    columns,
    dataList,
    selectedNum,
    pagination,
    buttonClass,
    deviceDetection,
    onSearch,
    resetForm,
    onbatchDel,
    handleUpdate,
    handleDelete,
    handleSizeChange,
    onSelectionCancel,
    handleCurrentChange,
    handleSelectionChange,
    sortByScoreAsc,
    sortByScoreDesc,
    sortByVotesAsc,
    sortByVotesDesc,
    approve,
    exportCurrent
  };
}
