<template>
  <div class="evaluate-detail">
    <div class="info">
      <span>
        <el-icon style="margin-right: 0.3em">
          <Calendar />
        </el-icon>
        {{ props.date }}
      </span>
      <span>
        评分：
        <el-rate
          v-if="props.score !== 0"
          :model-value="Number(props.score).toFixed(0)"
          disabled
          show-score
          text-color="#ff9900"
          size="small"
        />
        <el-text v-else style="font-size: 0.8rem; font-weight: 300"> 暂无评分 </el-text>
      </span>
    </div>
    <p>{{ props.suggestion }}</p>
    <div class="like-box">
      <el-button v-if="!visitingMode" type="primary" @click="like"
        >赞同 ({{ props.likeNum }})</el-button
      >
      <el-button v-else type="primary" @click="toHome">去完成入站考试后赞同</el-button>
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

const like = () => {
  emit('likeClick');
};

const emit = defineEmits(['likeClick']);

const props = defineProps({
  score: {
    type: Number,
    default: 0,
  },
  suggestion: {
    type: String,
    default: '',
  },
  date: {
    type: String,
    default: '',
  },
  likeNum: {
    type: Number,
    default: 0,
  },
});
</script>

<style scoped>
.evaluate-detail {
  padding: 0.2rem;
}

.evaluate-detail p {
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

.like-box {
  display: flex;
  justify-content: center;
  margin-top: 1rem;
}

:deep(.el-rate__icon) {
  margin-right: 1px;
}
</style>
