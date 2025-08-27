import axios from 'axios';
import type { AxiosResponse, InternalAxiosRequestConfig } from 'axios';
import { useUserStore } from '@/stores/user';
import { ElMessage } from 'element-plus';
import router from '@/router';
import FingerprintJS from '@fingerprintjs/fingerprintjs';
import { ref } from 'vue';

const LOGIN_API = '/api/UserEntrance/login'; // 登录接口路径

function getLocal(key: string) {
  return localStorage.getItem(key);
}

function setLocal(key: string, value: string) {
  localStorage.setItem(key, value);
}

function isTokenExpired(): boolean {
  const expires = getLocal('expires');
  if (!expires) return true;
  const now = new Date();
  const expireDate = new Date(expires);
  return now > expireDate;
}

// 登录并刷新token
async function refreshToken(): Promise<boolean> {
  const userId = getLocal('userId');
  const fastKey = getLocal('fastKey');
  const browserFingerprint = ref('');
  const fp = await FingerprintJS.load();
  const result = await fp.get();
  browserFingerprint.value = result.visitorId;
  try {
    const res: AxiosResponse = await axios.post(LOGIN_API, {
      userId,
      fastKey,
      browserFingerprint: browserFingerprint.value,
    });
    const { success, data } = res.data;
    if (success) {
      if (data.accessToken && data.expires) {
        setLocal('token', data.accessToken);
        setLocal('expires', data.expires);
        return true;
      }
      console.log('登录返回数据异常');
      ElMessage.error('系统错误');
      return false;
    } else {
      if (data && data.to === 'register') {
        console.log('需要注册');
        useUserStore().needRegister = true;
        if (router.currentRoute.value.path !== '/') {
          router.push({ path: '/' });
          ElMessage.error('请完成入站考试');
        }
      } else if (data && data.to === 'verify') {
        console.log('需要验证');
        useUserStore().needVerify = true;
        useUserStore().tempToken = data.tempToken;
        if (router.currentRoute.value.path !== '/') {
          router.push({ path: '/' });
          ElMessage.error('环境异常，请完成验证');
        }
      } else {
        console.log('登录失败');
        ElMessage.error('系统错误');
      }
      return false;
    }
  } catch (err: any) {
    console.log(err?.message || '登录失败');
    ElMessage.error('系统错误');
    return false;
  }
}

const instance = axios.create({
  headers: {
    Accept: 'application/json, text/plain, */*',
    'Content-Type': 'application/json',
  },
  timeout: 10000,
});

instance.interceptors.request.use(async (config: InternalAxiosRequestConfig) => {
  let token = getLocal('token');
  if ((!token || isTokenExpired()) && !useUserStore().visitingMode) {
    const refreshed = await refreshToken();
    if (refreshed) {
      token = getLocal('token');
    } else {
      throw new axios.Cancel('未登录或登录失败');
    }
  }
  if (token) {
    if (!config.headers) {
      config.headers = new axios.AxiosHeaders();
    }
    config.headers['Authorization'] = `Bearer ${token}`;
  }
  return config;
});

// 响应拦截
instance.interceptors.response.use(
  (response) => response,
  async (error) => {
    if (!axios.isCancel(error)) {
      console.log(error.message || '请求异常');
    }
    if (error.response) {
      return error.response;
    }
    return Promise.reject(error);
  },
);

export default instance;
