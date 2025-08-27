import { ref, onMounted, nextTick } from 'vue';
import { ElMessage } from 'element-plus';
import { getDishAddList, addDishAdd, addVote } from '@/api/canteen';

export function useDishAdd() {
  const isLoading = ref(true);
  const detailItemVisible = ref(false);
  const addItemVisible = ref(false);
  const isDishAddSubmitLoading = ref(false);
  const noMore = ref(false);
  const curId = ref();
  const items = ref<any[]>([]);
  const page = ref(1);
  const pageSize = 20;
  const totalPage = ref(0);
  const scrollContainer = ref<HTMLElement | null>(null);
  const voteNum = ref(0);

  const showDetailDialog = (id: number) => {
    voteNum.value = items.value.find((item) => item.id === id)?.vote || 0;
    curId.value = id;
    detailItemVisible.value = true;
  };

  const showAddItemDialog = () => {
    addItemVisible.value = true;
  };

  const onSearch = () => {
    page.value = 1;
    items.value = [];
    noMore.value = false;
    fetchItems();
  };

  const fetchItems = async () => {
    isLoading.value = true;
    const res = await getDishAddList({ page: page.value, pageSize });
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
    const res = await addVote({ id: curId.value, type: 1 });
    const { success, data } = res.data;
    if (success) {
      ElMessage.success('投票成功');
      voteNum.value++;
    } else {
      ElMessage.error(data.message);
    }
  };

  const handleDishAddSubmit = async (form: { name: string; description: string }) => {
    isDishAddSubmitLoading.value = true;
    const res = await addDishAdd({ ...form });
    const { success, data } = res.data;
    if (success) {
      ElMessage.success('提交成功');
      addItemVisible.value = false;
      onSearch();
    } else {
      ElMessage.error(data.message ?? '提交失败');
    }
    setTimeout(() => {
      isDishAddSubmitLoading.value = false;
    }, 500);
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
    addItemVisible,
    voteNum,
    isDishAddSubmitLoading,
    onSearch,
    handleScroll,
    showDetailDialog,
    showAddItemDialog,
    handleDishAddSubmit,
    vote,
  };
}
