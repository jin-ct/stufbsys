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

/** 获取建议列表 */
export const getSuggestionsList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/TeacherSuggestionCtrl/list", {
    data
  });
};

/** 删除建议 */
export const deleteSuggestions = (data?: object) => {
  return http.request<Result>("post", "/api/TeacherSuggestionCtrl/delete", {
    data
  });
};
