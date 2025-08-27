<template>
  <el-card class="item-card">
    <div class="item-card-content">
      <div class="item-card-info">
        <div class="item-card-title">{{ props.title }}</div>
        <div class="item-card-details">
          <p v-for="(value, key) in props.details" :key="key">{{ value }}</p>
        </div>
      </div>
      <div class="item-card-rate" v-if="props.score !== null">
        <el-rate
          v-if="props.score !== 0"
          :model-value="Number(props.score).toFixed(props.scoreFix)"
          disabled
          show-score
          text-color="#ff9900"
          size="small"
        />
        <el-text v-else style="font-size: 0.8rem; font-weight: 300"> 暂无评分 </el-text>
      </div>
      <div class="item-card-vote" v-if="props.vote !== null">
        <el-tag type="info">{{ props.vote }}票</el-tag>
      </div>
      <div class="item-btn">
        <el-button :type="props.btn['btn-type']" size="small" plain @click="$emit('btnClick')">
          {{ props.btn['btn-text'] }}
        </el-button>
      </div>
    </div>
  </el-card>
</template>

<script setup lang="ts">
interface BtnConfig {
  'btn-text': String;
  'btn-type'?: String;
}

const props = defineProps({
  title: {
    type: String,
    default: '',
  },
  details: {
    type: Object as () => Record<string, string>,
    default: () => ({}),
  },
  score: {
    type: Number,
    default: null,
  },
  scoreFix: {
    type: Number,
    default: 2,
  },
  vote: {
    type: Number,
    default: null,
  },
  btn: {
    type: Object as () => BtnConfig,
    default: () => ({ 'btn-text': '', 'btn-type': '' }),
  },
});
</script>

<style scoped>
.item-card {
  border-radius: 1rem;
  margin: 0.2rem 0.6rem;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.08);
  height: 4rem;
}

.item-card-content {
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.item-card-title {
  font-size: 0.95rem;
  margin-right: 0.8rem;
  display: -webkit-box;
  overflow: hidden;
  -webkit-line-clamp: 1;
  line-clamp: 1;
  -webkit-box-orient: vertical;
}

.item-card-details {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.item-card-details p {
  font-size: 0.78rem;
  color: #999;
  margin: 0;
  display: -webkit-box;
  overflow: hidden;
  -webkit-line-clamp: 1;
  line-clamp: 1;
  -webkit-box-orient: vertical;
}

.item-card-rate {
  flex: 1;
  display: flex;
  justify-content: flex-end;
  margin-right: 0.8rem;
}

.item-card-vote {
  flex: 1;
  display: flex;
  justify-content: flex-end;
  margin-right: 0.8rem;
}

:deep(.el-rate__icon) {
  margin-right: 1px;
}

:deep(.el-card__body) {
  padding: 0.8rem 0.8rem;
}
</style>
