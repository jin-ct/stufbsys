import { http } from "@/utils/http";

type Result = {
  success: boolean;
  data?: any;
};

/** 保存账号设置 */
export const saveAccountSettings = (data?: object) => {
  return http.request<Result>("post", "/api/TeacherSettings/set", { data });
};

/** 获取页脚处名言 */
export const getFooterSaying = () => {
  return http.request<Result>("get", "/api/TeacherSettings/saying/get");
};

/** 获取设置 */
export const getSettings = (data?: object) => {
  return http.request<Result>("post", "/api/TeacherSettings/get-settings", {
    data
  });
};

/** 管理员账号重置验证 */
export const verifyAdminReset = (data?: object) => {
  return http.request<Result>(
    "post",
    "/api/TeacherSettings/admin-reset-verify",
    { data }
  );
};
