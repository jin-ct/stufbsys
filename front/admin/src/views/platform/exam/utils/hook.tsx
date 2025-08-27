import "./reset.css";
import editForm from "../form/index.vue";
import { message } from "@/utils/message";
import { addDialog } from "@/components/ReDialog";
import type { PaginationProps } from "@pureadmin/table";
import type { FormItemProps } from "../utils/types";
import { getKeyList, deviceDetection } from "@pureadmin/utils";
import {
  getQuestionList,
  addQuestion,
  updateQuestion,
  deleteQuestion
} from "@/api/platform";
import { type Ref, h, ref, toRaw, computed, reactive, onMounted } from "vue";

export function useExam(tableRef: Ref) {
  const form = reactive({
    question: "",
    type: "-1"
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
      label: "题目",
      slot: "question",
      minWidth: 130
    },
    {
      label: "答案",
      minWidth: 90,
      prop: "answer"
    },
    {
      label: "类型",
      slot: "type",
      minWidth: 90
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
    const { success } = await deleteQuestion({ ids: [row.id] });
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
    const { success } = await deleteQuestion({
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
    const { success, data } = await getQuestionList({
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
    const question = ref("");
    const options = ref({
      A: "",
      B: "",
      C: "",
      D: ""
    });
    if (title == "修改") {
      if (row.type === 2) {
        question.value = row.question.split("^")[0];
        options.value = {
          A: row.question.split("^")[1].replace("[A]", ""),
          B: row.question.split("^")[2].replace("[B]", ""),
          C: row.question.split("^")[3].replace("[C]", ""),
          D: row.question.split("^")[4].replace("[D]", "")
        };
      } else {
        question.value = row.question;
      }
    }
    addDialog({
      title: `${title}题目`,
      props: {
        formInline: {
          question_id: row?.id ?? null,
          question: question,
          options: options,
          answer: row?.answer ?? "",
          type: row?.type ?? null
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
            let m_question = curData.question;
            if (curData.type == 2)
              m_question =
                curData.question +
                "^[A]" +
                curData.options.A +
                "^[B]" +
                curData.options.B +
                "^[C]" +
                curData.options.C +
                "^[D]" +
                curData.options.D;
            if (title === "添加") {
              const { success, data } = await addQuestion({
                question: m_question,
                answer: curData.answer,
                type: curData.type
              });
              chores(success, data.message);
            } else {
              const { success, data } = await updateQuestion({
                question_id: curData.question_id,
                question: m_question,
                answer: curData.answer,
                type: curData.type
              });
              chores(success, data.message);
            }
          }
        });
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
    handleDelete,
    handleSizeChange,
    onSelectionCancel,
    handleCurrentChange,
    handleSelectionChange
  };
}
