import { fileURLToPath, URL } from 'node:url';

import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import viteCompression from 'vite-plugin-compression';

// https://vite.dev/config/
export default defineConfig({
  server: {
    // 本地跨域代理 https://cn.vitejs.dev/config/server-options.html#server-proxy
    proxy: {
      '/api': {
        // 这里填写后端地址
        target: 'http://127.0.0.1:3000',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api/, ''),
      },
    },
  },
  base: '/',
  plugins: [
    vue(),
    viteCompression({
      filter: /\.(js|css|json|txt|ico|svg)(\?.*)?$/i,
      threshold: 1024, // 文件容量大于这个值进行压缩
      algorithm: 'gzip',
      ext: 'gz',
    }),
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url)),
    },
  },
});
