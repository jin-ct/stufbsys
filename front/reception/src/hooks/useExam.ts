import axios from 'axios';
import { ElMessage } from 'element-plus';
import { onMounted, reactive, ref } from 'vue';
import { useUserStore } from '@/stores/user';
import FingerprintJS from '@fingerprintjs/fingerprintjs';

export function useExam() {
  const questionsData = reactive({
    choices: [
      {
        question: {
          text: '',
          a: '',
          b: '',
          c: '',
          d: '',
        },
        question_id: null,
        answer: '',
      },
      {
        question: {
          text: '',
          a: '',
          b: '',
          c: '',
          d: '',
        },
        question_id: null,
        answer: '',
      },
    ],
    judges: [
      {
        question: '',
        question_id: null,
        answer: '',
      },
      {
        question: '',
        question_id: null,
        answer: '',
      },
      {
        question: '',
        question_id: null,
        answer: '',
      },
    ],
    fill: {
      question: '',
      question_id: null,
      answer: '',
    },
  });

  const isLoading = ref(false);
  const isSubmitBtnLoading = ref(false);
  const verifyImages = reactive(['']);
  const browserFingerprint = ref('');

  FingerprintJS.load()
    .then((fp) => fp.get())
    .then((result) => {
      browserFingerprint.value = result.visitorId;
    });

  const getQuestions = async () => {
    isLoading.value = true;
    try {
      const response = await axios.get('/api/UserEntrance/exam/get-questions');
      if (response.data.success) {
        response.data.data.choices.forEach((choice: any, idx: number) => {
          const parts = choice.question.split('^');
          questionsData.choices[idx].question.text = parts[0];
          questionsData.choices[idx].question.a = parts[1]?.substring(3) || '';
          questionsData.choices[idx].question.b = parts[2]?.substring(3) || '';
          questionsData.choices[idx].question.c = parts[3]?.substring(3) || '';
          questionsData.choices[idx].question.d = parts[4]?.substring(3) || '';
          questionsData.choices[idx].question_id = choice.question_id;
        });
        response.data.data.judges.forEach((judge: any, idx: number) => {
          questionsData.judges[idx].question = judge.question;
          questionsData.judges[idx].question_id = judge.question_id;
        });
        questionsData.fill.question = response.data.data.fill.question;
        questionsData.fill.question_id = response.data.data.fill.question_id;
      } else {
        ElMessage.error(response.data.data.message);
      }
    } catch (error) {
      console.error('Error fetching questions:', error);
      ElMessage.error('系统错误, 请刷新后重试');
    } finally {
      isLoading.value = false;
    }
  };

  const submitExam = async () => {
    isSubmitBtnLoading.value = true;
    let verifyImage_true = '';
    let verifyImage_false = '';
    for (let i = 0; i < questionsData.judges.length; i++) {
      if (questionsData.judges[i].answer === '对') {
        verifyImage_true = verifyImages[i];
      } else {
        verifyImage_false = verifyImages[i];
      }
    }
    try {
      const response = await axios.post('/api/UserEntrance/register', {
        ...questionsData,
        verifyImage_true,
        verifyImage_false,
        browserFingerprint: browserFingerprint.value,
      });
      if (response.data.success) {
        localStorage.setItem('userId', response.data.data.userId);
        localStorage.setItem('fastKey', response.data.data.fastKey);
        ElMessage.success('考试通过');
        useUserStore().needRegister = false;
      } else {
        ElMessage.error(response.data.data.message);
        getQuestions();
        ElMessage.info('已刷新试题，其重新作答');
      }
    } catch (error) {
      console.error('Error submitting exam:', error);
      ElMessage.error('系统错误, 请刷新后重试');
      throw error;
    } finally {
      isSubmitBtnLoading.value = false;
    }
  };

  const convertFillAnswerText = () => {
    const cnNumMap: Record<string, string> = {
      零: '0',
      一: '1',
      二: '2',
      三: '3',
      四: '4',
      五: '5',
      六: '6',
      七: '7',
      八: '8',
      九: '9',
    };

    if (questionsData.fill.answer && typeof questionsData.fill.answer === 'string') {
      // 转换小写字母为大写
      let answer = questionsData.fill.answer.replace(/[a-z]/g, (m) => m.toUpperCase());
      // 转换简体中文数字为阿拉伯数字
      answer = answer.replace(/[零一二三四五六七八九]/g, (m) => cnNumMap[m] || m);
      questionsData.fill.answer = answer;
    }
  };

  const visitingMode = () => {
    useUserStore().visitingMode = true;
    useUserStore().needRegister = false;
  };

  onMounted(() => {
    getQuestions();
  });

  return {
    questionsData,
    isLoading,
    isSubmitBtnLoading,
    verifyImages,
    submitExam,
    visitingMode,
    convertFillAnswerText,
  };
}
