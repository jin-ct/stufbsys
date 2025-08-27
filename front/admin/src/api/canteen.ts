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
  };
};

type exportAllEvaluatesResult = {
  success: boolean;
  data?: Array<{
    name: string;
    avg_score: number;
    evaluate_count: number;
    date: string;
    evaluates: Array<{
      id: number;
      score: number;
      vote_count: number;
      suggestion: string;
      date: string;
    }>;
  }>;
};

/** 获取菜品列表 */
export const getDishList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminCanteen/dish/list", {
    data
  });
};

/** 添加菜品 */
export const addDish = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/dish/add", { data });
};

/** 更新菜品 */
export const updateDish = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/dish/update", {
    data
  });
};

/** 删除菜品 */
export const deleteDish = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/dish/delete", {
    data
  });
};

/** 批量导入 */
export const importDishes = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/dish/import", {
    data
  });
};

/** 导出所有评价 */
export const exportAllEvaluates = (data?: object) => {
  return http.request<exportAllEvaluatesResult>(
    "post",
    "/api/AdminCanteen/evaluate/all",
    { data }
  );
};

/** 获取菜品评价列表 */
export const getEvaluateList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminCanteen/evaluate/list", {
    data
  });
};

/** 删除菜品评价 */
export const deleteEvaluate = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/evaluate/delete", {
    data
  });
};

/** 获取加菜列表 */
export const getDishAddList = (data?: object) => {
  return http.request<ResultTable>("post", "/api/AdminCanteen/dish/add/list", {
    data
  });
};

/** 删除加菜申请 */
export const deleteDishAdd = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/dish/add/delete", {
    data
  });
};

/** 审核加菜申请 */
export const approveDishAdd = (data?: object) => {
  return http.request<Result>("post", "/api/AdminCanteen/dish/add/approve", {
    data
  });
};
