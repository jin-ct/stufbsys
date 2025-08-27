<template>
  <div class="evaluate">
    <div class="rate-box">
      <span>综合评分</span>
      <el-rate v-model="score" text-color="#ff9900" size="large" :disabled="visitingMode" />
    </div>
    <div class="suggestion-box">
      <el-input
        :disabled="visitingMode"
        v-model="suggestion"
        type="textarea"
        placeholder="请输入改进建议（选填）"
        :rows="4"
        :maxlength="100"
        show-word-limit
      />
    </div>
    <div class="submit-box">
      <el-button type="primary" @click="handleSubmit" :loading="isLoading">{{
        visitingMode ? '去完成入站考试' : '提交评价'
      }}</el-button>
    </div>
    <div class="all-evaluate">
      <el-link type="primary" @click="$emit('viewAll')">查看该菜品所有评价</el-link>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import { ElMessage } from 'element-plus';
import { useUserStore } from '@/stores/user';
import router from '@/router';

const visitingMode = ref(useUserStore().visitingMode);
if (visitingMode.value) {
  ElMessage.warning('当前为访客模式');
}

const emit = defineEmits(['submit', 'viewAll']);

const score = defineModel<number>('score', { default: 0 });
const suggestion = defineModel<string>('suggestion', { default: '' });
const isLoading = defineModel<boolean>('isLoading', { default: false });

const handleSubmit = () => {
  if (visitingMode.value) {
    useUserStore().needRegister = true;
    router.push('/');
    return;
  }
  if (score.value === 0) {
    ElMessage.warning('请选择评分');
    return;
  }
  emit('submit', { score: score.value, suggestion: suggestion.value });
};
</script>

<style scoped>
.rate-box {
  display: flex;
  align-items: center;
  justify-content: flex-start;
  flex-direction: row;
  gap: 0.5rem;
}

.submit-box {
  display: flex;
  justify-content: center;
  margin-top: 1rem;
}

.all-evaluate {
  text-align: center;
  margin-top: 0.4rem;
}
</style>
