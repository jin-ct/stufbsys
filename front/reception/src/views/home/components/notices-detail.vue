<template>
  <div class="notices-detail">
    <p style="white-space: pre-wrap">{{ props.notice.detail }}</p>
    <div class="info">
      <span>
        <el-icon style="margin-right: 0.3em">
          <Calendar />
        </el-icon>
        {{ props.notice.date }}
      </span>
      <span>
        <el-icon style="margin-right: 0.3em">
          <User />
        </el-icon>
        发布人：{{ props.notice.publisher }}
      </span>
    </div>
  </div>
</template>

<script setup lang="ts">
const props = defineProps({
  notice: {
    type: Object as () => { detail: string; date: string; publisher: string },
    default: () => ({}),
  },
});
</script>

<style scoped>
.notices-detail {
  padding: 0.2rem;
}

.notices-detail p {
  font-size: 0.9rem;
  color: #333;
  margin: 0.2rem 0;
}

.info {
  display: flex;
  align-items: center;
  gap: 1rem;
  color: #888;
  font-size: 0.95em;
  margin-top: 0.5rem;
}

.info span {
  display: flex;
  align-items: center;
}
</style>
