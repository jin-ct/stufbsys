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

/** 获取公告列表 */
export const getNoticesList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminNotice/list", { data });
};

/** 添加公告 */
export const addNotice = (data?: object) => {
  return http.request<Result>("post", "/api/AdminNotice/add", { data });
};

/** 更新公告 */
export const updateNotice = (data?: object) => {
  return http.request<Result>("post", "/api/AdminNotice/update", { data });
};

/** 删除公告 */
export const deleteNotices = (data?: object) => {
  return http.request<Result>("post", "/api/AdminNotice/delete", { data });
};
