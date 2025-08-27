<template>
  <div class="fun-btn">
    <el-card class="fun-btn-card" @click="$emit('click')">
      <div>{{ text }}</div>
      <el-icon class="arrow-icon"><ArrowRight /></el-icon>
    </el-card>
  </div>
</template>

<script setup lang="ts">
const { text } = defineProps({
  text: {
    type: String,
    default: '',
  },
});
</script>

<style scoped>
.fun-btn {
  margin: 0.6rem 0.6rem 0.3rem 0.6rem;
}

.fun-btn-card {
  height: 100%;
  border-radius: 1rem;
  display: flex;
  align-items: center;
  justify-content: space-between;
  cursor: pointer;
  padding-right: 1rem;
  position: relative;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.08) !important;
}

.fun-btn-title {
  font-size: 0.8rem;
  color: #333;
}

.arrow-icon {
  font-size: 1rem;
  position: absolute;
  right: 1rem;
  top: 50%;
  transform: translateY(-50%);
}
</style>
