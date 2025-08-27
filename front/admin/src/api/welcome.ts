import { http } from "@/utils/http";

/** 获取统计数据 */
export const getStatisticsData = () => {
  return http.request<any>("get", "/api/AdminWelcome/get");
};
