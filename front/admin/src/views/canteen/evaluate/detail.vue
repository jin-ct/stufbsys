<script setup lang="ts">
import { ref } from "vue";
import { useEvaluate } from "./utils/evaluateHook";
import { PureTableBar } from "@/components/RePureTableBar";
import { useRenderIcon } from "@/components/ReIcon/src/hooks";

import Delete from "~icons/ep/delete";

defineOptions({
  name: "EvaluateDetail"
});

const props = defineProps({
  id: {
    type: Number,
    required: true
  },
  name: {
    type: String,
    default: ""
  }
});

const tableRef = ref();

const {
  id,
  loading,
  columns,
  dataList,
  selectedNum,
  pagination,
  resetForm,
  onbatchDel,
  handleDelete,
  handleSizeChange,
  onSelectionCancel,
  handleCurrentChange,
  handleSelectionChange,
  sortByScoreAsc,
  sortByScoreDesc,
  onlyShowSuggestions
} = useEvaluate(tableRef, props.id);
</script>

<template>
  <div class="main">
    <PureTableBar
      :title="`菜品评价 - ${props.name}`"
      :columns="columns"
      @refresh="resetForm"
    >
      <template #buttons>
        <el-button type="primary" @click="onlyShowSuggestions()">
          只看有改进建议的评价
        </el-button>
        <el-dropdown>
          <el-button class="ml-3! mt-[2px]!">按分数排序</el-button>
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item>
                <el-button
                  link
                  type="primary"
                  :icon="useRenderIcon('ep:sort-down')"
                  @click="sortByScoreAsc()"
                >
                  低分在前
                </el-button>
              </el-dropdown-item>
              <el-dropdown-item>
                <el-button
                  link
                  type="primary"
                  :icon="useRenderIcon('ep:sort-up')"
                  @click="sortByScoreDesc()"
                >
                  高分在前
                </el-button>
              </el-dropdown-item>
            </el-dropdown-menu>
          </template>
        </el-dropdown>
      </template>
      <template v-slot="{ size, dynamicColumns }">
        <div
          v-if="selectedNum > 0"
          v-motion-fade
          class="bg-[var(--el-fill-color-light)] w-full h-[46px] mb-2 pl-4 flex items-center"
        >
          <div class="flex-auto">
            <span
              style="font-size: var(--el-font-size-base)"
              class="text-[rgba(42,46,54,0.5)] dark:text-[rgba(220,220,242,0.5)]"
            >
              已选 {{ selectedNum }} 项
            </span>
            <el-button type="primary" text @click="onSelectionCancel">
              取消选择
            </el-button>
          </div>
          <el-popconfirm title="是否确认删除?" @confirm="onbatchDel">
            <template #reference>
              <el-button type="danger" text class="mr-1!"> 批量删除 </el-button>
            </template>
          </el-popconfirm>
        </div>
        <pure-table
          ref="tableRef"
          row-key="id"
          adaptive
          :adaptiveConfig="{ offsetBottom: 140 }"
          align-whole="center"
          table-layout="auto"
          :loading="loading"
          :size="size"
          :data="dataList"
          :columns="dynamicColumns"
          :pagination="{ ...pagination, size }"
          :header-cell-style="{
            background: 'var(--el-fill-color-light)',
            color: 'var(--el-text-color-primary)'
          }"
          @selection-change="handleSelectionChange"
          @page-size-change="handleSizeChange"
          @page-current-change="handleCurrentChange"
        >
          <template #score="{ row }">
            <el-rate
              v-model="row.score"
              disabled
              allow-half
              :show-score="true"
              size="small"
              class="text-xs"
            />
          </template>
          <template #suggestion="{ row }">
            <el-tooltip
              v-if="row.suggestion && row.suggestion.length > 30"
              :content="row.suggestion"
              placement="top"
              popper-class="notice-title-popper"
              :disabled="!row.suggestion"
            >
              <div
                style="
                  overflow: hidden;
                  text-overflow: ellipsis;
                  white-space: nowrap;
                "
              >
                {{ row.suggestion.slice(0, 30) + "..." }}
              </div>
            </el-tooltip>
            <div
              v-else
              style="
                overflow: hidden;
                text-overflow: ellipsis;
                white-space: nowrap;
              "
            >
              {{ row.suggestion }}
            </div>
          </template>
          <template #operation="{ row }">
            <el-popconfirm
              :title="`是否确认删除ID为${row.id}的这条数据`"
              @confirm="handleDelete(row)"
            >
              <template #reference>
                <el-button
                  class="reset-margin"
                  link
                  type="primary"
                  :size="size"
                  :icon="useRenderIcon(Delete)"
                >
                  删除
                </el-button>
              </template>
            </el-popconfirm>
          </template>
        </pure-table>
      </template>
    </PureTableBar>
  </div>
</template>

<style lang="scss" scoped>
:deep(.el-dropdown-menu__item i) {
  margin: 0;
}

:deep(.el-button:focus-visible) {
  outline: none;
}

.main-content {
  margin: 24px 24px 0 !important;
}

.search-form {
  :deep(.el-form-item) {
    margin-bottom: 12px;
  }
}
</style>
