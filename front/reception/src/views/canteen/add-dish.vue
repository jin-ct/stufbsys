<template>
  <div class="main-container">
    <div class="content">
      <pageTitle title="加菜列表" />
      <div class="items" ref="scrollContainer" @scroll="handleScroll" id="scroll-container">
        <div class="item" v-for="item in items" :key="item.id">
          <itemCard
            :title="item.name"
            :details="{ 'ai-score': `AI评分：${Number(item.score).toFixed(2)}` }"
            :vote="item.vote"
            :btn="{ 'btn-text': '详情', 'btn-type': 'primary' }"
            @click="showDetailDialog(item.id)"
          />
        </div>
        <div v-if="isLoading" class="loading">正在加载...</div>
        <div v-if="noMore" class="loading">没有更多了</div>
        <scrollTop />
      </div>
      <bottomBtn :btnNum="1" :btn1="{ 'btn-text': '发起加菜投票' }" @click="showAddItemDialog" />
    </div>
    <Dialog v-model="detailItemVisible" title="菜品详情">
      <addDishDetail
        :vote="voteNum"
        :date="items.find((item) => item.id === curId).date"
        :name="items.find((item) => item.id === curId).name"
        :description="items.find((item) => item.id === curId).description"
        @vote="vote"
      />
    </Dialog>
    <Dialog v-model="addItemVisible" title="发起加菜投票">
      <addDishVote @submit="handleDishAddSubmit" v-model:isLoading="isDishAddSubmitLoading" />
    </Dialog>
  </div>
</template>

<script setup lang="ts">
import bottomBtn from '@/components/bottom-btn.vue';
import itemCard from '@/components/item-card.vue';
import pageTitle from '@/components/page-title.vue';
import Dialog from '@/components/dialog.vue';
import addDishDetail from './components/add-dish-detail.vue';
import addDishVote from './components/add-dish-vote.vue';
import scrollTop from '@/components/scroll-top.vue';
import { useSettings } from '@/hooks/useSettings';
import { useDishAdd } from '@/hooks/useDishAdd';

const { settings } = useSettings();
const {
  isLoading,
  noMore,
  curId,
  items,
  scrollContainer,
  detailItemVisible,
  addItemVisible,
  voteNum,
  isDishAddSubmitLoading,
  handleScroll,
  showDetailDialog,
  showAddItemDialog,
  handleDishAddSubmit,
  vote,
} = useDishAdd();
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
  padding-top: 0.4rem;
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
