import { reactive } from "vue";
import type { FormRules } from "element-plus";

/** 自定义表单规则校验 */
export const formRules = reactive(<FormRules>{
  type: [{ required: true, message: "题目类型为必选项", trigger: "blur" }],
  question: [{ required: true, message: "题目为必填项", trigger: "blur" }],
  answer: [{ required: true, message: "答案为必填项", trigger: "blur" }]
});
