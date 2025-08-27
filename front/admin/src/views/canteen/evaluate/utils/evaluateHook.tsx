import "./reset.css";
import dayjs from "dayjs";
import { message } from "@/utils/message";
import type { PaginationProps } from "@pureadmin/table";
import { getKeyList, deviceDetection } from "@pureadmin/utils";
import { getEvaluateList, deleteEvaluate } from "@/api/canteen";
import { type Ref, h, ref, toRaw, computed, reactive, onMounted } from "vue";

export function useEvaluate(tableRef: Ref, dishId: number) {
  const dataList = ref([]);
  const id = dishId;
  const sortByScore = ref("");
  const isNotSuggestionNull = ref(false);
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
      label: "分数",
      slot: "score",
      minWidth: 90
    },
    {
      label: "赞同",
      prop: "vote_count",
      width: 90
    },
    {
      label: "改进建议",
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
      width: 90,
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
    const { success } = await deleteEvaluate({ ids: [row.id] });
    if (success) {
      message(`删除成功`, { type: "success" });
    } else {
      message(`删除失败`, { type: "error" });
      return;
    }
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

  function sortByScoreAsc() {
    sortByScore.value = "asc";
    onSearch();
  }

  function sortByScoreDesc() {
    sortByScore.value = "desc";
    onSearch();
  }

  function onlyShowSuggestions() {
    isNotSuggestionNull.value = !isNotSuggestionNull.value;
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
    const { success } = await deleteEvaluate({
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
    const { success, data } = await getEvaluateList({
      dish_id: id,
      pageSize: pagination.pageSize,
      toPage: pagination.currentPage,
      sortByScore: sortByScore.value,
      isNotSuggestionNull: isNotSuggestionNull.value
    });
    dataList.value = data.list;
    pagination.total = data.total;

    if (success) {
      dataList.value = data.list;
      pagination.total = data.total;
    }

    setTimeout(() => {
      loading.value = false;
    }, 500);
  }

  function resetForm() {
    sortByScore.value = "";
    isNotSuggestionNull.value = false;
    onSearch();
  }

  onMounted(async () => {
    onSearch();
  });

  return {
    id,
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
    handleDelete,
    handleSizeChange,
    onSelectionCancel,
    handleCurrentChange,
    handleSelectionChange,
    sortByScoreAsc,
    sortByScoreDesc,
    onlyShowSuggestions
  };
}
