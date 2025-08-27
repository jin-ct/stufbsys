<template>
  <div class="verification">
    <Loading v-if="isSubmitBtnLoading" />
    <div class="tip">
      <span>请证明你是人类</span>
    </div>
    <div class="judge-question">
      <div class="items">
        <div class="item">
          <div class="questions">
            <div class="question">① {{ questionsData.questions[0].question }}</div>
            <div class="question">② {{ questionsData.questions[1].question }}</div>
            <div class="question">③ {{ questionsData.questions[2].question }}</div>
          </div>
          <div>
            <div class="judge-answers">
              <Painting
                promptText="1"
                v-model:base64="questionsData.questions[0].verifyImage"
                v-model:answer="questionsData.questions[0].userAnswer"
                @drawingEnd="submitVerify"
              />
              <Painting
                promptText="2"
                v-model:base64="questionsData.questions[1].verifyImage"
                v-model:answer="questionsData.questions[1].userAnswer"
                @drawingEnd="submitVerify"
              />
              <Painting
                promptText="3"
                v-model:base64="questionsData.questions[2].verifyImage"
                v-model:answer="questionsData.questions[2].userAnswer"
                @drawingEnd="submitVerify"
              />
            </div>
            <div class="tip">请判断正误并画出“✓”或“×”</div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import Painting from '@/components/painting.vue';
import { useVerify } from '@/hooks/useVerify';
import Loading from '@/components/loading.vue';

const { questionsData, isSubmitBtnLoading, submitVerify } = useVerify();
</script>

<style scoped>
.tip {
  text-align: center;
  font-size: 0.8rem;
  color: rgb(115.2, 117.6, 122.4);
}

.items {
  margin-left: 0.5rem;
}

.questions {
  display: flex;
  gap: 1rem;
  font-size: 0.95rem;
  justify-content: center;
}

.question {
  margin-bottom: 0.3rem;
  margin-top: 0.3rem;
}

.judge-question {
  margin-top: 0.4rem;
}

.judge-answers {
  display: flex;
  gap: 0.5rem;
  align-items: center;
  height: 2.8rem;
  margin-top: 0.7rem;
  margin-bottom: 0.2rem;
}

.submit-box {
  display: flex;
  justify-content: center;
  margin-top: 0.5rem;
}
</style>
