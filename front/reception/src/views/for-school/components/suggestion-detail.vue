<template>
  <div class="suggestion-detail">
    <div class="info">
      <span v-if="props.name !== ''">
        <el-icon style="margin-right: 0.3em">
          <User />
        </el-icon>
        {{ props.name }}
      </span>
      <span v-if="props.class !== ''">
        <el-icon style="margin-right: 0.3em">
          <School />
        </el-icon>
        {{ props.class }}
      </span>
      <span v-if="props.grade !== ''">
        {{ props.grade }}
      </span>
    </div>
    <p><span class="tip">标题：</span>{{ props.title }}</p>
    <p v-if="!isLong"><span class="tip">内容：</span>{{ props.suggestion }}</p>
    <div class="content-long" v-if="isLong"><slot></slot></div>
    <div class="info" style="margin: 0.5rem 0">
      <span>
        <el-icon style="margin-right: 0.3em">
          <Calendar />
        </el-icon>
        {{ props.date }}
      </span>
      <span v-if="props.typeName !== ''">
        <el-icon style="margin-right: 0.3em">
          <ChatDotRound />
        </el-icon>
        {{ props.typeName }}
      </span>
      <span>
        <el-icon style="margin-right: 0.3em">
          <Star />
        </el-icon>
        AI评分：{{ Number(props.aiScore).toFixed(2) }}
      </span>
    </div>
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
  name: {
    type: String,
    default: '',
  },
  class: {
    type: String,
    default: '',
  },
  grade: {
    type: String,
    default: '',
  },
  title: {
    type: String,
    default: '',
  },
  suggestion: {
    type: String,
    default: '',
  },
  date: {
    type: String,
    default: '',
  },
  typeName: {
    type: String,
    default: '',
  },
  aiScore: {
    type: Number,
    default: 0,
  },
  likeNum: {
    type: Number,
    default: 0,
  },
  isLong: {
    type: Boolean,
    default: false,
  },
});
</script>

<style scoped>
.suggestion-detail {
  padding: 0.2rem;
}

.suggestion-detail p {
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

.tip {
  color: #888;
  font-weight: bold;
}

.like-box {
  display: flex;
  justify-content: center;
  margin-top: 0.5rem;
}

.content-long {
  margin-top: 0.5rem;
  padding: 0.3rem;
  border-radius: 0.2rem;
  border: rgb(221.7, 222.6, 224.4) 1px solid;
  max-height: 24rem;
  overflow-y: auto;
  position: relative;
}

/* 进度条样式 */
.content-long::-webkit-scrollbar {
  width: 6px;
  background: #f2f2f2;
  border-radius: 3px;
}
.content-long::-webkit-scrollbar-thumb {
  background: #c1c1c1;
  border-radius: 3px;
}
.content-long::-webkit-scrollbar-thumb:hover {
  background: #a8a8a8;
}

:deep(.el-rate__icon) {
  margin-right: 1px;
}
</style>
