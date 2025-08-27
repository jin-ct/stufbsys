<template>
  <div class="title-item">
    <div
      :class="['title-item-content', { covered: showText }]"
      :style="{ '--imgSrc': `url(${bannerUrl})` }"
    >
      <h4 v-if="showText">{{ school }}学生反馈平台</h4>
    </div>
  </div>
</template>

<script setup lang="ts">
import { defineProps } from 'vue';

const { school, bannerUrl, showText } = defineProps({
  school: {
    type: String,
    default: '',
  },

  bannerUrl: {
    type: String,
    default: '',
  },

  showText: {
    type: Boolean,
    default: false,
  },
});
</script>

<style scoped>
.title-item {
  margin-bottom: -3.5rem;
  overflow: hidden;
  width: 100%;
}

.title-item .covered {
  background-image:
    linear-gradient(to bottom, rgba(0, 0, 0, 0.1) 0%, rgba(0, 0, 0, 0.05) 66%, #f2f3f5 100%),
    var(--imgSrc);
  background-position: center;
  background-size: cover;
  background-repeat: no-repeat;
  padding: 1.5rem;
}

.title-item-content {
  background-image:
    linear-gradient(to bottom, rgba(0, 0, 0, 0) 0%, rgba(0, 0, 0, 0) 66%, #f2f3f5 100%),
    var(--imgSrc);
  background-position: center;
  background-size: cover;
  background-repeat: no-repeat;
  padding: 1.5rem;
}

.title-item-content h4 {
  font-weight: 400;
  font-size: 1.2rem;
  color: white;
  text-align: center;
}

@media screen and (max-width: 767px) {
  .title-item-content {
    height: 6rem;
  }
  .title-item-content h4 {
    margin-top: 1rem;
  }
}

@media screen and (min-width: 768px) {
  .title-item-content {
    height: 5rem;
  }
  .title-item-content h4 {
    margin-top: 0.3rem;
  }
}
</style>
