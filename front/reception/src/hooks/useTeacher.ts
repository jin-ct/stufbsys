import { ref, onMounted, nextTick } from 'vue';
import router from '@/router';
import { ElMessage } from 'element-plus';
import { getTeacherList } from '@/api/tchSuggestion';

export function useTeacher() {
  const isLoading = ref(true);
  const noMore = ref(false);
  const items = ref<any[]>([]);
  const page = ref(1);
  const pageSize = 20;
  const totalPage = ref(0);
  const scrollContainer = ref<HTMLElement | null>(null);
  const searchContent = ref('');

  const choseTeacher = (id: number) => {
    router.push({ name: 'for-teacher-list', params: { id } });
  };

  const onSearch = () => {
    page.value = 1;
    items.value = [];
    noMore.value = false;
    fetchItems();
  };

  const fetchItems = async () => {
    isLoading.value = true;
    const res = await getTeacherList({ page: page.value, pageSize, search: searchContent.value });
    const { success, data } = res.data;
    if (success) {
      if (data.total === 0) {
        noMore.value = true;
        isLoading.value = false;
        return;
      }
      if (data.total) totalPage.value = Math.ceil(data.total / pageSize);
      items.value.push(...data.teachers);
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

  onMounted(async () => {
    await nextTick();
    fetchItems();
  });

  return {
    isLoading,
    noMore,
    items,
    searchContent,
    scrollContainer,
    onSearch,
    handleScroll,
    choseTeacher,
  };
}
