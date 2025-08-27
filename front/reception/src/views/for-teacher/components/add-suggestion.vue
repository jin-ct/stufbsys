<template>
  <div class="add-suggestion">
    <div class="title-box">
      <el-input
        v-model="title"
        placeholder="请输入标题"
        :maxlength="40"
        show-word-limit
        :disabled="visitingMode"
      />
    </div>
    <div class="content-box">
      <el-input
        :disabled="visitingMode"
        v-model="content"
        type="textarea"
        placeholder="请输入建议内容"
        :rows="5"
        :maxlength="500"
        show-word-limit
      />
    </div>
    <div class="info-box" v-if="!isAnonymous">
      <el-input v-model="name" placeholder="姓名" :maxlength="10" show-word-limit />
      <el-input v-model="className" placeholder="班级" :maxlength="10" show-word-limit />
    </div>
    <div class="setting-box">
      <span>设置：</span>
      <div>
        <el-checkbox label="公开" v-model="isPublic" :disabled="visitingMode">公开</el-checkbox>
        <el-checkbox label="匿名" v-model="isAnonymous" :disabled="visitingMode">匿名</el-checkbox>
      </div>
    </div>
    <div class="submit-box">
      <el-button v-if="!visitingMode" type="primary" @click="handleSubmit" :loading="isLoading"
        >提交建议</el-button
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

const title = defineModel<string>('title', { default: '' });
const content = defineModel<string>('content', { default: '' });
const name = defineModel<string>('name', { default: '' });
const className = defineModel<string>('className', { default: '' });
const isPublic = defineModel<boolean>('public', { default: true });
const isAnonymous = defineModel<boolean>('anonymous', { default: true });

const isLoading = defineModel<boolean>('isLoading', { default: false });

const handleSubmit = () => {
  const formData = {
    title: title.value,
    content: content.value,
    name: name.value,
    className: className.value,
    isPublic: isPublic.value,
    isAnonymous: isAnonymous.value,
  };
  if (formData.title.trim() === '' || formData.content.trim() === '') {
    return ElMessage.warning('请填写必要字段');
  }
  if (!formData.isAnonymous && !formData.name && !formData.className) {
    return ElMessage.warning('请填写个人信息');
  }
  emit('submit', formData);
};
</script>

<style scoped>
.title-box {
  margin: 0.6rem 0;
}

.content-box {
  margin-bottom: 0.5rem;
}

.submit-box {
  display: flex;
  justify-content: center;
  margin-top: 0.5rem;
}

.info-box {
  margin: 0.5rem 0;
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.setting-box {
  margin: 0.5rem 0;
  display: flex;
  align-items: center;
}
</style>
