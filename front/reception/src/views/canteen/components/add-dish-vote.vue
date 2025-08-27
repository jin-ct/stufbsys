<template>
  <div class="add-dish-vote">
    <div class="name-box">
      <el-input
        v-model="name"
        placeholder="请输入菜品名称"
        :maxlength="20"
        show-word-limit
        :disabled="visitingMode"
      />
    </div>
    <div class="description-box">
      <el-input
        v-model="description"
        :disabled="visitingMode"
        type="textarea"
        placeholder="请输入详细的描述，如原料、烹饪方式、口味等"
        :rows="4"
        :maxlength="100"
        show-word-limit
      />
    </div>
    <span>*需通过学校审核通过才可公开投票</span>
    <div class="submit-box">
      <el-button v-if="!visitingMode" type="primary" @click="handleSubmit" :loading="isLoading"
        >提交申请</el-button
      >
      <el-button v-else type="primary" @click="toHome">去完成入站考试</el-button>
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

const toHome = () => {
  useUserStore().needRegister = true;
  router.push('/');
};

const emit = defineEmits(['submit']);

const name = defineModel<string>('name', { default: '' });
const description = defineModel<string>('description', { default: '' });
const isLoading = defineModel<boolean>('isLoading', { default: false });

const handleSubmit = () => {
  if (name.value.trim() === '' || description.value.trim() === '') {
    ElMessage.warning('请填写必要字段');
    return;
  }
  emit('submit', { name: name.value, description: description.value });
};
</script>

<style scoped>
.name-box {
  margin: 0.6rem 0;
}

.description-box {
  margin-bottom: 0.5rem;
}

.submit-box {
  display: flex;
  justify-content: center;
  margin-top: 0.5rem;
}
</style>
