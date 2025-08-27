import { http } from "@/utils/http";

type Result = {
  success: boolean;
  data?: {
    message: string;
  };
};

type ResultTable = {
  success: boolean;
  data?: {
    /** 列表数据 */
    list: Array<any>;
    /** 总条目数 */
    total?: number;
    /** 每页显示条目个数 */
    pageSize?: number;
    /** 当前页数 */
    currentPage?: number;
  };
};

/** 获取考试问题列表 */
export const getQuestionList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminPlatform/exam/list", {
    data
  });
};

/** 添加考试问题 */
export const addQuestion = (data?: object) => {
  return http.request<Result>("post", "/api/AdminPlatform/exam/add", { data });
};

/** 更新考试问题 */
export const updateQuestion = (data?: object) => {
  return http.request<Result>("post", "/api/AdminPlatform/exam/update", {
    data
  });
};

/** 删除考试问题 */
export const deleteQuestion = (data?: object) => {
  return http.request<Result>("post", "/api/AdminPlatform/exam/delete", {
    data
  });
};

/** 保存设置项 */
export const saveSettings = (data?: object) => {
  return http.request<Result>("post", "/api/AdminPlatform/setting/set", {
    data
  });
};

/** 获取设置项 */
export const getSettings = () => {
  return http.request<any>("post", "/api/AdminPlatform/setting/get", {});
};

/** 上传Logo */
export const uploadLogo = (data?: FormData) => {
  return http.request<Result>(
    "post",
    "/api/AdminPlatform/setting/upload/logo",
    { data },
    { headers: { "Content-Type": "multipart/form-data" } }
  );
};

/** 上传HomeImage */
export const uploadHomeImage = (data?: FormData) => {
  return http.request<Result>(
    "post",
    "/api/AdminPlatform/setting/upload/home/image",
    { data },
    { headers: { "Content-Type": "multipart/form-data" } }
  );
};

/** 设置管理员密码 */
export const setAdminPassword = (data?: object) => {
  return http.request<Result>(
    "post",
    "/api/AdminPlatform/setting/admin/set/password",
    { data }
  );
};
