<template>
  <div class="main-container">
    <div class="content">
      <pageTitle title="公开的建议列表" />
      <div class="items" ref="scrollContainer" @scroll="handleScroll" id="scroll-container">
        <searchInput placeholder="请搜索标题或日期" v-model="searchContent" @search="onSearch" />
        <div class="item" v-for="item in items" :key="item.id">
          <itemCard
            :title="item.title"
            :details="{
              date: item.date,
              'ai-score': `AI评分：${item.score === 0 ? '等待评分' : Number(item.score).toFixed(2)}`,
            }"
            :btn="{ 'btn-text': '详情', 'btn-type': 'primary' }"
            @btnClick="showDetailDialog(item.id)"
          />
        </div>
        <div v-if="isLoading" class="loading">正在加载...</div>
        <div v-if="noMore" class="loading">没有更多了</div>
        <scrollTop />
      </div>
      <bottomBtn
        :btnNum="1"
        :btn1="{ 'btn-text': '写建议', 'btn-type': 'primary' }"
        @btn1Click="showAddItemDialog"
      />
    </div>
    <Dialog v-model="detailItemVisible" title="建议详情">
      <suggestionDetail
        :is-long="true"
        :name="items.find((item) => item.id === curId)?.name"
        :class="items.find((item) => item.id === curId)?.class"
        :grade="items.find((item) => item.id === curId)?.grade"
        :title="items.find((item) => item.id === curId)?.title"
        :date="items.find((item) => item.id === curId)?.date"
        :ai-score="items.find((item) => item.id === curId)?.score"
        :typeName="typeNameMap[items.find((item) => item.id === curId)?.type] || ''"
        :like-num="voteNum"
        @like-click="vote"
      >
        <div v-html="items.find((item) => item.id === curId)?.content"></div>
      </suggestionDetail>
    </Dialog>
    <Dialog v-model="addItemVisible" title="写建议">
      <addSuggestion
        :isLong="true"
        v-model:is-loading="isAddSubmitLoading"
        @submit="handleAddSubmit"
      />
    </Dialog>
  </div>
</template>

<script setup lang="ts">
import bottomBtn from '@/components/bottom-btn.vue';
import itemCard from '@/components/item-card.vue';
import pageTitle from '@/components/page-title.vue';
import searchInput from '@/components/search-input.vue';
import Dialog from '@/components/dialog.vue';
import suggestionDetail from './components/suggestion-detail.vue';
import addSuggestion from './components/add-suggestion.vue';
import scrollTop from '@/components/scroll-top.vue';
import { useSettings } from '@/hooks/useSettings';
import { useSchSuggestionLong } from '@/hooks/useSchSuggestionLong';

const typeNameMap: Record<number, string> = {
  0: '饭堂',
  1: '教学',
  2: '环境',
  3: '其他',
};

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
  isAddSubmitLoading,
  searchContent,
  onSearch,
  handleScroll,
  showDetailDialog,
  showAddItemDialog,
  handleAddSubmit,
  vote,
} = useSchSuggestionLong();
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
