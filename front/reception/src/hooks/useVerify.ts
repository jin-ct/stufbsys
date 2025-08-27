import { ElMessage } from 'element-plus';
import { onMounted, reactive, ref } from 'vue';
import { useUserStore } from '@/stores/user';
import axios from 'axios';
import router from '@/router';

export function useVerify() {
  const questionsData = reactive({
    questions: [
      {
        question: '',
        answer: true,
        userAnswer: '',
        verifyImage: '',
      },
      {
        question: '',
        answer: true,
        userAnswer: '',
        verifyImage: '',
      },
      {
        // 第三道题不传回服务器，用于防止与入站考试的绘制框尺寸不一致
        question: '',
        answer: true,
        userAnswer: '',
        verifyImage: '',
      },
    ],
  });

  const isSubmitBtnLoading = ref(false);

  const getQuestions = async () => {
    const operators = ['+', '-'];
    const generateQuestion = () => {
      const num1 = Math.floor(Math.random() * 11);
      const num2 = Math.floor(Math.random() * 11);
      const operator = operators[Math.floor(Math.random() * operators.length)];
      const correctResult = operator === '+' ? num1 + num2 : num1 - num2;
      const showResult =
        Math.random() < 0.5 ? correctResult : correctResult + (Math.random() < 0.5 ? 1 : -1);
      return {
        question: `${num1} ${operator} ${num2} = ${showResult}`,
        answer: correctResult === showResult,
      };
    };

    questionsData.questions = questionsData.questions.map(() => ({
      ...generateQuestion(),
      userAnswer: '',
      verifyImage: '',
    }));
  };

  const submitVerify = async () => {
    if (
      questionsData.questions[0].userAnswer === '' ||
      questionsData.questions[1].userAnswer === '' ||
      questionsData.questions[2].userAnswer === ''
    ) {
      return;
    }
    if (questionsData.questions.some((q) => (q.userAnswer === '对') !== q.answer)) {
      ElMessage.error('验证失败！');
      getQuestions();
      ElMessage.info('已刷新');
      isSubmitBtnLoading.value = false;
      return;
    }
    isSubmitBtnLoading.value = true;
    try {
      const res = await axios.post('/api/UserEntrance/verify', {
        tempToken: useUserStore().tempToken,
        ...questionsData,
      });
      const { success, data } = res.data;
      if (success) {
        localStorage.setItem('userId', data.userId);
        localStorage.setItem('fastKey', data.fastKey);
        ElMessage.success('验证通过');
        useUserStore().needRegister = false;
        useUserStore().needVerify = false;
      } else if (data.to == 'register') {
        useUserStore().needRegister = true;
        useUserStore().needVerify = false;
        if (router.currentRoute.value.path !== '/') {
          router.push({ path: '/' });
        }
      }
    } catch (error) {
      console.error('Error submitting verify:', error);
      ElMessage.error('系统错误, 请刷新后重试');
      throw error;
    } finally {
      isSubmitBtnLoading.value = false;
    }

    setTimeout(() => {
      isSubmitBtnLoading.value = false;
    }, 600);
  };

  onMounted(() => {
    getQuestions();
  });

  return {
    questionsData,
    isSubmitBtnLoading,
    submitVerify,
  };
}
