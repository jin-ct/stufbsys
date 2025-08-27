<template>
  <div class="exam">
    <Loading v-if="isLoading" />
    <div class="tip">
      <span>为了确保你是本校学生，请完成以下测试</span>
    </div>
    <div class="single-choice">
      <div class="title">单选题</div>
      <div class="items">
        <div class="item" v-for="(value, index) in questionsData.choices" :key="index">
          <div class="question">{{ index + 1 }}. {{ value.question.text }}</div>
          <el-select v-model="value.answer" placeholder="请选择">
            <el-option :label="value.question.a" value="A"></el-option>
            <el-option :label="value.question.b" value="B"></el-option>
            <el-option :label="value.question.c" value="C"></el-option>
            <el-option :label="value.question.d" value="D"></el-option>
          </el-select>
        </div>
      </div>
    </div>
    <div class="judge-question">
      <div class="title">判断题</div>
      <div class="items">
        <div class="item">
          <div class="question" v-for="(value, index) in questionsData.judges" :key="index">
            {{ index + 1 }}. {{ value.question }}
          </div>
          <div>
            <div class="tip">请在下方画出“✓”或“×”</div>
            <div class="judge-answers">
              <Painting
                promptText="1"
                v-model:answer="questionsData.judges[0].answer"
                v-model:base64="verifyImages[0]"
              />
              <Painting
                promptText="2"
                v-model:answer="questionsData.judges[1].answer"
                v-model:base64="verifyImages[1]"
              />
              <Painting
                promptText="3"
                v-model:answer="questionsData.judges[2].answer"
                v-model:base64="verifyImages[2]"
              />
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="fill-in-the-blank">
      <div class="title">填空题</div>
      <div class="items">
        <div class="item">
          <div class="question">{{ questionsData.fill.question }}</div>
          <el-input
            v-model="questionsData.fill.answer"
            placeholder="请输入答案"
            :onchange="convertFillAnswerText"
          ></el-input>
        </div>
      </div>
    </div>
    <div class="tip-2">
      <span>*同一浏览器仅第一次进入时需进行测试</span>
      <span>*为避免刷票等行为，后续进入若检测到环境异常，将需使用判断题答案进行人机验证</span>
    </div>
    <div class="submit-box">
      <el-button type="primary" @click="submitExam" :loading="isSubmitBtnLoading"
        >提交并进入</el-button
      >
      <el-button type="primary" @click="visitingMode" plain>参观模式</el-button>
    </div>
  </div>
</template>

<script setup lang="ts">
import Painting from '@/components/painting.vue';
import Loading from '@/components/loading.vue';
import { useExam } from '@/hooks/useExam';

const {
  questionsData,
  isLoading,
  isSubmitBtnLoading,
  verifyImages,
  submitExam,
  visitingMode,
  convertFillAnswerText,
} = useExam();
</script>

<style scoped>
.tip {
  text-align: center;
  font-size: 0.8rem;
  color: rgb(115.2, 117.6, 122.4);
}

.single-choice {
  margin-top: 0.5rem;
}

.title {
  font-size: 0.9rem;
  font-weight: 500;
}

.items {
  margin-left: 0.5rem;
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
  margin: 0.2rem 0;
}

.fill-in-the-blank {
  margin-top: 0.5rem;
}

.tip-2 {
  font-size: 0.7rem;
  margin: 0.4rem 0;
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  line-height: 1.6;
}

.submit-box {
  display: flex;
  justify-content: center;
  margin-top: 0.5rem;
}
</style>
