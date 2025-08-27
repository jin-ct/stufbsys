import { http } from "@/utils/http";

/** 获取统计数据 */
export const getStatisticsData = data => {
  return http.request<any>("post", "/api/TeacherWelcome/get", { data });
};
