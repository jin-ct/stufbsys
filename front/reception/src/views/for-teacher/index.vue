<template>
  <div class="main-container">
    <div class="content">
      <pageTitle title="请选择教师" />
      <div class="items" ref="scrollContainer" @scroll="handleScroll" id="scroll-container">
        <searchInput
          placeholder="请搜索姓名/年级/班级/科目"
          v-model="searchContent"
          @search="onSearch"
        />
        <div class="item" v-for="item in items" :key="item.id">
          <itemCard
            :title="item.name"
            :details="{ subject: item.subject, class: item.class, grade: item.grade }"
            :btn="{ 'btn-text': '选择', 'btn-type': 'primary' }"
            @btnClick="choseTeacher(item.id)"
          />
        </div>
        <div v-if="isLoading" class="loading">正在加载...</div>
        <div v-if="noMore" class="loading">没有更多了</div>
        <scrollTop />
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import searchInput from '@/components/search-input.vue';
import itemCard from '@/components/item-card.vue';
import pageTitle from '@/components/page-title.vue';
import scrollTop from '@/components/scroll-top.vue';
import { useSettings } from '@/hooks/useSettings';
import { useTeacher } from '@/hooks/useTeacher';

const { settings } = useSettings();
const {
  isLoading,
  noMore,
  items,
  searchContent,
  scrollContainer,
  onSearch,
  handleScroll,
  choseTeacher,
} = useTeacher();
</script>

<style scoped>
.content {
  display: flex;
  flex-direction: column;
  height: 100%;
}

.items {
  flex: 1;
  overflow-y: auto;
}

.item {
  margin-bottom: 0.5rem;
}

.loading {
  text-align: center;
  font-size: 0.8rem;
  padding: 0.2rem;
  color: #999;
}

@media screen and (max-width: 767px) {
  .main-container {
    width: 100%;
    height: 100vh;
    background-color: #f2f3f5;
  }

  .fixed-footer {
    flex-direction: column;
    display: flex;
    align-items: center;
    justify-content: center;
  }
}

@media screen and (min-width: 768px) {
  .main-container {
    background-color: #f2f3f5;
    width: 100%;
    height: 100vh;
    display: flex;
  }

  .main-container .content {
    width: 460px;
    height: 560px;
    margin: auto;
    background-color: #fafafa;
    border-radius: 18px;
    box-shadow: 0 4px 24px rgba(0, 0, 0, 0.04);
    overflow: hidden;
    position: relative;
  }
}
</style>
