import http from '@/utils/http';

/** 获取短建议列表 */
export const getShortSuggestionList = (params: any) => {
  return http.get('/api/UserSuggestion/short/list', { params });
};

/** 添加短建议 */
export const addShortSuggestion = (data: any) => {
  return http.post('/api/UserSuggestion/short/add', data);
};

/** 获取长建议列表 */
export const getLongSuggestionList = (params: any) => {
  return http.get('/api/UserSuggestion/long/list', { params });
};

/** 添加长建议 */
export const addLongSuggestion = (data: any) => {
  return http.post('/api/UserSuggestion/long/add', data);
};

/** 添加投票 */
export const addVote = (params: any) => {
  return http.get('/api/UserSuggestion/vote', { params });
};
