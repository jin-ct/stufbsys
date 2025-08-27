import { ref, onMounted, nextTick } from 'vue';
import { ElMessage } from 'element-plus';
import { getDishEvaluationList, addVote } from '@/api/canteen';

export function useEvaluate(id: number) {
  const isLoading = ref(true);
  const detailItemVisible = ref(false);
  const noMore = ref(false);
  const dishId = ref(id);
  const curId = ref();
  const items = ref<any[]>([]);
  const page = ref(1);
  const pageSize = 20;
  const totalPage = ref(0);
  const scrollContainer = ref<HTMLElement | null>(null);
  const likeNum = ref(0);

  const showDetailDialog = (id: number) => {
    likeNum.value = items.value.find((item) => item.id === id)?.vote || 0;
    curId.value = id;
    detailItemVisible.value = true;
  };

  const onSearch = () => {
    page.value = 1;
    items.value = [];
    noMore.value = false;
    fetchItems();
  };

  const fetchItems = async () => {
    isLoading.value = true;
    const res = await getDishEvaluationList({ page: page.value, pageSize, dishId: dishId.value });
    const { success, data } = res.data;
    if (success) {
      if (data.total === 0) {
        noMore.value = true;
        isLoading.value = false;
        return;
      }
      if (data.total) {
        totalPage.value = Math.ceil(data.total / pageSize);
      }
      items.value.push(...data.evaluates);
      isLoading.value = false;
    }
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

  const vote = async () => {
    if (!curId.value) return;
    const res = await addVote({ id: curId.value, type: 0 });
    const { success, data } = res.data;
    if (success) {
      ElMessage.success('赞同成功');
      likeNum.value++;
    } else {
      ElMessage.error(data.message == '你已投过票' ? '你已赞同过' : data.message);
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
    scrollContainer,
    detailItemVisible,
    likeNum,
    onSearch,
    handleScroll,
    showDetailDialog,
    vote,
  };
}
