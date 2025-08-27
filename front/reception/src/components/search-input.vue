<template>
  <div class="search-input-item">
    <el-input v-model="content" class="search-input" :placeholder="placeholder" clearable>
      <template #append>
        <el-button :icon="Search" @click="emit('search')" />
      </template>
    </el-input>
  </div>
</template>

<script setup lang="ts">
import { ElInput, ElButton } from 'element-plus';
import { Search } from '@element-plus/icons-vue';

const emit = defineEmits<{
  (e: 'search'): void;
}>();

const content = defineModel<string | number>();
const { placeholder } = defineProps({
  placeholder: {
    type: String,
    default: '请输入搜索内容',
  },
});
</script>

<style scoped>
.search-input-item {
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
}

.search-input {
  width: 100%;
  height: 2.4rem;
  margin: 0.6rem;
}

:deep(.el-input__wrapper) {
  border-radius: 0.8rem !important;
}

:deep(.el-input-group__append) {
  border-radius: 0.8rem !important;
  margin-left: 0.3rem;
}
</style>
