import http from '@/utils/http';

/** 获取教师列表 */
export const getTeacherList = (params: any) => {
  return http.get('/api/UserTchSuggestion/teacher/list', { params });
};

/** 获取建议列表 */
export const getSuggestionList = (params: any) => {
  return http.get('/api/UserTchSuggestion/suggestion/list', { params });
};

/** 添加建议 */
export const addSuggestion = (data: any) => {
  return http.post('/api/UserTchSuggestion/suggestion/add', data);
};

/** 添加投票 */
export const addVote = (params: any) => {
  return http.get('/api/UserTchSuggestion/vote', { params });
};
