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

/** 获取教师列表 */
export const getTeacherList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminTeacher/list", { data });
};

/** 添加教师 */
export const addTeacher = (data?: object) => {
  return http.request<Result>("post", "/api/AdminTeacher/add", { data });
};

/** 更新教师 */
export const updateTeacher = (data?: object) => {
  return http.request<Result>("post", "/api/AdminTeacher/update", { data });
};

/** 删除教师 */
export const deleteTeacher = (data?: object) => {
  return http.request<Result>("post", "/api/AdminTeacher/delete", { data });
};

/** 批量导入 */
export const importTeachers = (data?: object) => {
  return http.request<Result>("post", "/api/AdminTeacher/import", { data });
};

/** 重置密码 */
export const resetPassword = (data?: object) => {
  return http.request<Result>("post", "/api/AdminTeacher/reset", { data });
};
