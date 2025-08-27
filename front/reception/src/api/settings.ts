import axios from 'axios';
import http from '@/utils/http';

/** 获取网站基本设置 */
export const getSettings = () => {
  return axios.get('/api/UserEntrance/get-settings');
};

/** 增加访问量 */
export const incrementVisitCount = () => {
  return http.get('/api/UserEntrance/increment-visit');
};
