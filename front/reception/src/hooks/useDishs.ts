import { ref, onMounted, nextTick } from 'vue';
import { ElMessage } from 'element-plus';
import { getDishList, evaluateDish } from '@/api/canteen';

export function useDishs() {
  const isLoading = ref(true);
  const noMore = ref(false);
  const evaluateItemVisible = ref(false);
  const isEvaluateSubmitLoading = ref(false);
  const curId = ref();
  const items = ref<any[]>([]);
  const page = ref(1);
  const pageSize = 20;
  const totalPage = ref(0);
  const scrollContainer = ref<HTMLElement | null>(null);
  const searchContent = ref('');

  const showEvaluateDialog = (id: number) => {
    curId.value = id;
    evaluateItemVisible.value = true;
  };

  const onSearch = () => {
    page.value = 1;
    items.value = [];
    noMore.value = false;
    fetchItems();
  };

  const fetchItems = async () => {
    isLoading.value = true;
    const res = await getDishList({ page: page.value, pageSize, search: searchContent.value });
    const { success, data } = res.data;
    if (success) {
      if (data.total === 0) {
        noMore.value = true;
        isLoading.value = false;
        return;
      }
      if (data.total) totalPage.value = Math.ceil(data.total / pageSize);
      items.value.push(...data.dishes);
      isLoading.value = false;
    }
  };

  const handleEvaluateSubmit = async (form: { score: number; suggestion: string }) => {
    if (!curId.value) return;
    isEvaluateSubmitLoading.value = true;
    const res = await evaluateDish({ id: curId.value, ...form });
    const { success, data } = res.data;
    if (success) {
      ElMessage.success('评价提交成功');
      evaluateItemVisible.value = false;
      onSearch();
    } else {
      ElMessage.error(data.message);
    }
    setTimeout(() => {
      isEvaluateSubmitLoading.value = false;
    }, 500);
  };

  const handleScroll = () => {
    if (isLoading.value || noMore.value) return;
    const el = scrollContainer.value;
    if (!el) return;
    // 距底部100px时触发
    if (el.scrollTop + el.clientHeight >= el.scrollHeight - 100) {
      if (totalPage.value !== 0 && page.value > totalPage.value) {
        noMore.value = true;
        isLoading.value = false;
        return;
      }
      page.value++;
      fetchItems();
    }
  };

  onMounted(async () => {
    await nextTick();
    fetchItems();
  });

  return {
    isLoading,
    noMore,
    curId,
    items,
    searchContent,
    scrollContainer,
    evaluateItemVisible,
    isEvaluateSubmitLoading,
    onSearch,
    handleScroll,
    showEvaluateDialog,
    handleEvaluateSubmit,
  };
}
