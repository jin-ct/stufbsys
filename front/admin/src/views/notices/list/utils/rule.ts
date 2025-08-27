import { reactive } from "vue";
import type { FormRules } from "element-plus";

/** 自定义表单规则校验 */
export const formRules = reactive(<FormRules>{
  publisher: [
    { required: true, message: "发布人名称为必填项", trigger: "blur" }
  ],
  show_text: [
    { required: true, message: "首页展示文本为必填项", trigger: "blur" }
  ]
});
