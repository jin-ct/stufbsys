<template>
  <div class="add-dish-detail">
    <div class="info">
      <span>
        <el-icon style="margin-right: 0.3em">
          <Calendar />
        </el-icon>
        {{ props.date }}
      </span>
      <span> {{ props.vote }}票 </span>
    </div>
    <p><span class="tip">名称：</span>{{ props.name }}</p>
    <p><span class="tip">描述：</span>{{ props.description }}</p>
    <div class="vote-box">
      <el-button v-if="!visitingMode" type="primary" @click="$emit('vote')">投票</el-button>
      <el-button v-else type="primary" @click="toHome">去完成入站考试后投票</el-button>
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

const emit = defineEmits(['vote']);

const props = defineProps({
  vote: {
    type: Number,
    default: 0,
  },
  name: {
    type: String,
    default: '',
  },
  description: {
    type: String,
    default: '',
  },
  date: {
    type: String,
    default: '',
  },
});
</script>

<style scoped>
.add-dish-detail {
  padding: 0.2rem;
}

.add-dish-detail p {
  font-size: 0.9rem;
  color: #333;
  margin: 0.2rem 0;
}

.info {
  display: flex;
  align-items: center;
  gap: 0.6rem;
  color: #888;
  font-size: 0.95em;
  margin-bottom: 0.5rem;
}

.info span {
  display: flex;
  align-items: center;
}

.vote-box {
  display: flex;
  justify-content: center;
  margin-top: 1rem;
}

.tip {
  color: #888;
  font-weight: bold;
}

:deep(.el-rate__icon) {
  margin-right: 1px;
}
</style>
