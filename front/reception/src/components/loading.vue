<template>
  <div class="loading-overlay">
    <div class="loading-box">
      <el-icon class="loading-icon" :size="32">
        <Loading class="is-loading" />
      </el-icon>
    </div>
  </div>
</template>

<style scoped>
.loading-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 9999;
}

.loading-box {
  background: rgba(255, 255, 255, 0.95);
  border-radius: 1rem;
  box-shadow: 0 2px 16px rgba(0, 0, 0, 0.08);
  padding: 2rem;
  display: flex;
  align-items: center;
  justify-content: center;
}

.loading-icon {
  color: #409eff;
}

.is-loading {
  animation: spin 2s linear infinite;
}

@keyframes spin {
  100% {
    transform: rotate(360deg);
  }
}
</style>
