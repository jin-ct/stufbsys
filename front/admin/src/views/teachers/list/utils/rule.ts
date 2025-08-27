import { reactive } from "vue";
import type { FormRules } from "element-plus";
import { isPhone, isEmail } from "@pureadmin/utils";

/** 自定义表单规则校验 */
export const formRules = reactive(<FormRules>{
  name: [{ required: true, message: "教师姓名为必填项", trigger: "blur" }],
  subject: [{ required: true, message: "科目为必填项", trigger: "blur" }],
  grade: [{ required: true, message: "年级为必填项", trigger: "blur" }],
  class: [{ required: true, message: "班级为必填项", trigger: "blur" }],
  email: [
    {
      validator: (rule, value, callback) => {
        if (value === "") {
          callback();
        } else if (!isEmail(value)) {
          callback(new Error("请输入正确的邮箱格式"));
        } else {
          callback();
        }
      },
      trigger: "blur"
    }
  ]
});
