<template>
  <div class="main-container">
    <div class="content">
      <pageTitle title="请选择建议类型" />
      <functionBtn
        text="简单几句"
        style="height: 3.2rem; margin-top: 1.2rem"
        @click="router.push({ name: 'for-school-short-list' })"
      />
      <functionBtn
        text="长篇大论"
        style="height: 3.2rem; margin-top: 0.8rem"
        @click="router.push({ name: 'for-school-long-list' })"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import { useSettings } from '@/hooks/useSettings';
import router from '@/router';
import pageTitle from '@/components/page-title.vue';
import functionBtn from '@/components/function-btn.vue';

const { settings } = useSettings();
</script>

<style scoped>
@media screen and (max-width: 767px) {
  .main-container {
    width: 100%;
    height: 100vh;
    background-color: #f2f3f5;
  }

  .fixed-footer {
    flex-direction: column;
    display: flex;
    align-items: center;
    justify-content: center;
  }
}

@media screen and (min-width: 768px) {
  .main-container {
    background-color: #f2f3f5;
    width: 100%;
    height: 100vh;
    display: flex;
  }

  .main-container .content {
    width: 460px;
    height: 560px;
    margin: auto;
    background-color: #fafafa;
    border-radius: 18px;
    box-shadow: 0 4px 24px rgba(0, 0, 0, 0.04);
    overflow: hidden;
  }
}
</style>
