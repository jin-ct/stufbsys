import { http } from "@/utils/http";

type Result = {
  success: boolean;
  data?: Array<any>;
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

/** 获取短建议列表 */
export const getShortSuggestionsList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminSuggestion/short/list", {
    data
  });
};

/** 删除短建议 */
export const deleteShortSuggestion = (data?: object) => {
  return http.request<Result>("post", "/api/AdminSuggestion/short/delete", {
    data
  });
};

/** 获取长建议列表 */
export const getLongSuggestionsList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminSuggestion/long/list", {
    data
  });
};

/** 删除长建议 */
export const deleteLongSuggestion = (data?: object) => {
  return http.request<Result>("post", "/api/AdminSuggestion/long/delete", {
    data
  });
};
