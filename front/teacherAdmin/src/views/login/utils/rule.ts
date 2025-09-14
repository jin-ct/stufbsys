import { reactive } from "vue";
import type { FormRules } from "element-plus";

/** 密码正则（密码应为8-18位） */
export const REGEXP_PWD = /^.{8,18}$/;

/** 登录校验 */
const loginRules = reactive<FormRules>({
  password: [
    {
      validator: (rule, value, callback) => {
        if (value === "") {
          callback(new Error("请输入密码"));
        } else if (!REGEXP_PWD.test(value)) {
          callback(
            new Error("密码应为8-18位")
          );
        } else {
          callback();
        }
      },
      trigger: "blur"
    }
  ]
});

export { loginRules };
