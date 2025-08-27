<template>
  <div class="page-title">
    <el-icon class="back-icon" @click="$router.back()">
      <arrow-left />
    </el-icon>
    <div class="title">
      {{ title }}
    </div>
  </div>
</template>

<script setup lang="ts">
const { title } = defineProps({
  title: {
    type: String,
    default: '',
  },
});
</script>

<style scoped>
.page-title {
  width: 100%;
  height: 2.8rem;
  display: flex;
  align-items: center;
  justify-content: center;
  background-color: #fafafa;
  border-bottom: 1px solid #e8e8e8;
  font-size: 0.9rem;
  position: relative;
}

.back-icon {
  position: absolute;
  left: 1rem;
  cursor: pointer;
  display: flex;
  align-items: center;
}
</style>
