import http from '@/utils/http';

/** 获取菜品列表 */
export const getDishList = (params: any) => {
  return http.get('/api/UserCanteen/dish/list', { params });
};

/** 评价菜品 */
export const evaluateDish = (data: any) => {
  return http.post('/api/UserCanteen/dish/evaluate', data);
};

/** 获取评价列表 */
export const getDishEvaluationList = (params: any) => {
  return http.get('/api/UserCanteen/evaluate/list', { params });
};

/** 获取加菜列表 */
export const getDishAddList = (params: any) => {
  return http.get('/api/UserCanteen/dish-add/list', { params });
};

/** 申请加菜 */
export const addDishAdd = (data: any) => {
  return http.post('/api/UserCanteen/dish-add/add', data);
};

/** 投票 */
export const addVote = (params: any) => {
  return http.get('/api/UserCanteen/vote', { params });
};
