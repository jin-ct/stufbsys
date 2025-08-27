import "./reset.css";
import editForm from "../form/index.vue";
import importfromExcel from "../form/import-from-excel.vue";
import { message } from "@/utils/message";
import { addDialog } from "@/components/ReDialog";
import type { PaginationProps } from "@pureadmin/table";
import type { FormItemProps } from "../utils/types";
import { getKeyList, deviceDetection } from "@pureadmin/utils";
import {
  getTeacherList,
  addTeacher,
  updateTeacher,
  deleteTeacher,
  importTeachers,
  resetPassword
} from "@/api/teacher";
import { type Ref, h, ref, toRaw, computed, reactive, onMounted } from "vue";

export function useTeacher(tableRef: Ref) {
  const form = reactive({
    content: ""
  });
  const formRef = ref();
  const importFormRef = ref();
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
      label: "姓名",
      prop: "name",
      minWidth: 90
    },
    {
      label: "科目",
      prop: "subject",
      minWidth: 90
    },
    {
      label: "年级",
      prop: "grade",
      minWidth: 90
    },
    {
      label: "班级",
      prop: "class",
      minWidth: 100
    },
    {
      label: "邮箱",
      prop: "email",
      minWidth: 100
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

  async function handleReset(row) {
    const { success, data } = await resetPassword({ id: row.id });
    if (success) {
      message(data.message, { type: "success" });
    } else {
      message(`重置失败`, { type: "error" });
      return;
    }
  }

  async function handleDelete(row) {
    const { success } = await deleteTeacher({ ids: [row.id] });
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
    const { success } = await deleteTeacher({
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
    const { success, data } = await getTeacherList({
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
      title: `${title}教师`,
      props: {
        formInline: {
          title,
          teacher_id: row?.id ?? null,
          name: row?.name ?? "",
          subject: row?.subject ?? "",
          grade: row?.grade ?? "",
          class: row?.class ?? "",
          email: row?.email ?? ""
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
            // console.log("curData", curData);
            if (title === "添加") {
              const { success, data } = await addTeacher(curData);
              chores(success, data.message);
            } else {
              const { success, data } = await updateTeacher(curData);
              chores(success, data.message);
            }
          }
        });
      }
    });
  }

  function importFromExcel() {
    addDialog({
      title: "导入Excel表格",
      props: {
        formInline: {
          title: "导入Excel表格",
          isCover: false,
          data: ""
        }
      },
      width: "46%",
      draggable: true,
      fullscreen: deviceDetection(),
      fullscreenIcon: true,
      closeOnClickModal: false,
      contentRenderer: () =>
        h(importfromExcel, { ref: importFormRef, formInline: null }),
      beforeSure: async (done, { options }) => {
        const curData = options.props.formInline;
        if (curData.isReady) {
          const { success, data } = await importTeachers({
            teachers: curData.data,
            isCover: curData.isCover
          });
          if (success) {
            message("Excel表格导入成功", { type: "success" });
          } else {
            message(`导入失败：${data.message}`, { type: "error" });
          }
          done(); // 关闭弹框
          onSearch();
        } else {
          message("未选择有效文件，若解析失败请重新导入", { type: "warning" });
        }
      }
    });
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
    handleReset,
    handleDelete,
    handleSizeChange,
    onSelectionCancel,
    handleCurrentChange,
    handleSelectionChange,
    importFromExcel
  };
}
