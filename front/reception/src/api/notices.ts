import http from '@/utils/http';

/** 获取公告 */
export const getNotices = () => {
  return http.get('/api/UserEntrance/get-notices');
};
