<template>
  <div class="main-container">
    <div class="content">
      <pageTitle title="饭堂菜品评价" />
      <div class="items" ref="scrollContainer" @scroll="handleScroll" id="scroll-container">
        <searchInput
          placeholder="请搜索菜品名称或日期"
          v-model="searchContent"
          @search="onSearch"
        />
        <div class="item" v-for="item in items" :key="item.id">
          <itemCard
            :title="item.name"
            :details="{ date: item.date }"
            :score="item.avg_score"
            :btn="{ 'btn-text': '评价', 'btn-type': 'primary' }"
            @btnClick="showEvaluateDialog(item.id)"
          />
        </div>
        <div v-if="isLoading" class="loading">正在加载...</div>
        <div v-if="noMore" class="loading">没有更多了</div>
        <scrollTop />
      </div>
      <bottomBtn
        v-if="settings.is_show_dish_add_entrance"
        :btnNum="1"
        :btn1="{ 'btn-text': '投票加菜' }"
        @btn1Click="router.push({ name: 'canteen-add-dish' })"
      />
    </div>
    <Dialog v-model="evaluateItemVisible" title="菜品评价">
      <evaluate
        :isLoading="isEvaluateSubmitLoading"
        @viewAll="router.push({ name: 'canteen-evaluate-list', params: { id: curId } })"
        @submit="handleEvaluateSubmit"
      />
    </Dialog>
  </div>
</template>

<script setup lang="ts">
import router from '@/router';
import searchInput from '@/components/search-input.vue';
import bottomBtn from '@/components/bottom-btn.vue';
import itemCard from '@/components/item-card.vue';
import pageTitle from '@/components/page-title.vue';
import Dialog from '@/components/dialog.vue';
import evaluate from './components/evaluate.vue';
import scrollTop from '@/components/scroll-top.vue';
import { useSettings } from '@/hooks/useSettings';
import { useDishs } from '@/hooks/useDishs';

const { settings } = useSettings();
const {
  isLoading,
  noMore,
  curId,
  items,
  scrollContainer,
  searchContent,
  evaluateItemVisible,
  isEvaluateSubmitLoading,
  handleScroll,
  showEvaluateDialog,
  onSearch,
  handleEvaluateSubmit,
} = useDishs();
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
  padding-bottom: 4rem;
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
