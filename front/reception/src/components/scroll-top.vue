<template>
  <transition name="fade">
    <el-button
      v-show="visible"
      class="scroll-top-btn"
      type="primary"
      circle
      size="large"
      @click="scrollToTop"
      :icon="ElIconArrowUp"
      aria-label="返回顶部"
    />
  </transition>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue';
import { ElButton } from 'element-plus';
import { ArrowUp as ElIconArrowUp } from '@element-plus/icons-vue';

const visible = ref(false);

const scrollContainerId = 'scroll-container';
let scrollContainer: HTMLElement | null = null;

const handleScroll = () => {
  if (!scrollContainer) return;
  const scrollTop = scrollContainer.scrollTop;
  visible.value = scrollTop > 200;
};

const scrollToTop = () => {
  if (!scrollContainer) return;
  scrollContainer.scrollTo({ top: 0, behavior: 'smooth' });
};

onMounted(() => {
  scrollContainer = document.getElementById(scrollContainerId);
  if (scrollContainer) {
    scrollContainer.addEventListener('scroll', handleScroll, { passive: true });
    handleScroll();
  }
});

onUnmounted(() => {
  if (scrollContainer) {
    scrollContainer.removeEventListener('scroll', handleScroll);
  }
});
</script>

<style scoped>
.scroll-top-btn {
  position: fixed;
  right: 20px;
  bottom: 38px;
  z-index: 9999;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
  border: 1px solid #ccc;
  background: linear-gradient(
    135deg,
    rgba(255, 255, 255, 0.75) 70%,
    rgba(248, 248, 248, 0.75) 100%
  );
  color: #666;
  transition:
    box-shadow 0.2s,
    transform 0.2s,
    background 0.2s,
    color 0.2s;
  font-size: 16px;
  width: 36px;
  height: 36px;
  backdrop-filter: blur(2px);
}
.scroll-top-btn:hover {
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.15);
  transform: translateY(-2px) scale(1.05);
  background: linear-gradient(
    135deg,
    rgba(245, 245, 245, 0.92) 70%,
    rgba(255, 255, 255, 0.92) 100%
  );
  color: #222;
  border-color: #bbb;
}
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.3s;
}
.fade-enter-from,
.fade-leave-to {
  opacity: 0;
}
</style>
