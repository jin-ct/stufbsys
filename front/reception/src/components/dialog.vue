<template>
  <el-dialog
    v-model="visible"
    width="94%"
    style="max-width: 30rem; border-radius: 0.6rem; max-height: 95vh"
    :close-on-click-modal="false"
    :show-close="isShowClose"
    align-center
    destroy-on-close
  >
    <div class="dialog-header">
      <h5>{{ title }}</h5>
    </div>
    <div class="dialog-content">
      <slot />
    </div>
  </el-dialog>
</template>

<script setup lang="ts">
const visible = defineModel<boolean>({
  default: false,
  type: Boolean,
});

const { title, isShowClose } = defineProps({
  title: {
    type: String,
    default: '',
  },
  isShowClose: {
    type: Boolean,
    default: true,
  },
});
</script>

<style scoped>
.dialog-header {
  display: flex;
  align-items: center;
  justify-content: center;
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  margin: 0.64rem 3rem;
  text-align: center;
}

.dialog-header h5 {
  margin: 0;
  text-align: center;
  font-size: 0.95rem;
  font-weight: 600;
}

.dialog-content {
  max-height: calc(95vh - 38px);
  overflow-y: auto;
  position: relative;
  padding: 0 2px; /* 留出内容与滚动条间隙 */
}

/* 滚动条样式 */
.dialog-content::-webkit-scrollbar {
  width: 4px;
  background: #f2f2f2;
  border-radius: 1px;
}
.dialog-content::-webkit-scrollbar-thumb {
  background: #c1c1c1;
  border-radius: 1px;
}
.dialog-content::-webkit-scrollbar-thumb:hover {
  background: #a8a8a8;
}
</style>
