import "./reset.css";
import dayjs from "dayjs";
import editForm from "../form/index.vue";
import { message } from "@/utils/message";
import { addDialog } from "@/components/ReDialog";
import type { PaginationProps } from "@pureadmin/table";
import type { FormItemProps } from "../utils/types";
import { getKeyList, deviceDetection } from "@pureadmin/utils";
import {
  getNoticesList,
  addNotice,
  updateNotice,
  deleteNotices
} from "@/api/notices";
import { type Ref, h, ref, toRaw, computed, reactive, onMounted } from "vue";

export function useNotices(tableRef: Ref) {
  const form = reactive({
    content: "",
    date: ["", ""]
  });
  const formRef = ref();
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
      width: 60
    },
    {
      label: "发布人",
      prop: "publisher",
      minWidth: 80
    },
    {
      label: "首页展示文本",
      slot: "text",
      minWidth: 130
    },
    {
      label: "详情",
      slot: "detail",
      minWidth: 130
    },
    {
      label: "状态",
      slot: "states",
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

  function handleUpdate(row) {
    console.log(row);
  }

  async function handleDelete(row) {
    const { success } = await deleteNotices({ ids: [row.id] });
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
    const { success } = await deleteNotices({
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
    const { success, data } = await getNoticesList({
      search: toRaw(form),
      pageSize: pagination.pageSize,
      toPage: pagination.currentPage
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
    onSearch();
  };

  onMounted(async () => {
    onSearch();
  });

  function openDialog(title = "添加", row?) {
    addDialog({
      title: `${title}公告`,
      props: {
        formInline: {
          title,
          notice_id: row?.id ?? null,
          publisher: row?.publisher ?? "管理员",
          show_text: row?.text ?? "",
          detail: row?.detail ?? "",
          is_show: row?.state ?? true,
          date: row?.date ?? dayjs().format("YYYY-MM-DD")
        }
      },
      width: "46%",
      draggable: true,
      fullscreen: deviceDetection(),
      fullscreenIcon: true,
      closeOnClickModal: false,
      contentRenderer: () => h(editForm, { ref: formRef, formInline: null }),
      beforeSure: (done, { options }) => {
        const FormRef = formRef.value.getRef();
        const curData = options.props.formInline as FormItemProps;
        if (curData.detail === "") curData.detail = curData.show_text;
        function chores(isSuccess = true, errMessage = "") {
          if (isSuccess) {
            message(`${title}成功`, {
              type: "success"
            });
          } else {
            message(`添加失败：${errMessage}`, {
              type: "error"
            });
          }
          done(); // 关闭弹框
          onSearch(); // 刷新表格数据
        }
        FormRef.validate(async valid => {
          if (valid) {
            console.log("curData", curData);
            if (title === "添加") {
              const { success, data } = await addNotice(curData);
              chores(success, data.message);
            } else {
              const { success, data } = await updateNotice(curData);
              chores(success, data.message);
            }
          }
        });
      }
    });
  }

  async function switchShow(row) {
    const { success, data } = await updateNotice({
      notice_id: row.id,
      is_show: row.state === true ? false : true
    });
    if (success) {
      message(`操作成功`, {
        type: "success"
      });
    } else {
      message(`操作失败：${data.message}`, {
        type: "error"
      });
    }
    onSearch();
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
    openDialog,
    handleUpdate,
    handleDelete,
    handleSizeChange,
    onSelectionCancel,
    handleCurrentChange,
    handleSelectionChange,
    switchShow
  };
}
