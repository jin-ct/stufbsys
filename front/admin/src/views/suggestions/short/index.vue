<script setup lang="ts">
import { ref } from "vue";
import { useSuggestions } from "./utils/hook";
import { PureTableBar } from "@/components/RePureTableBar";
import { useRenderIcon } from "@/components/ReIcon/src/hooks";

import Refresh from "~icons/ep/refresh";
import Delete from "~icons/ep/delete";

defineOptions({
  name: "ShortSuggestion"
});

const formRef = ref();
const tableRef = ref();

const {
  form,
  loading,
  columns,
  dataList,
  pagination,
  selectedNum,
  onSearch,
  resetForm,
  onbatchDel,
  onSelectionCancel,
  handleDelete,
  handleSizeChange,
  handleCurrentChange,
  handleSelectionChange,
  openDetailDialog,
  exportCurrent,
  sortByScoreAsc,
  sortByScoreDesc
} = useSuggestions(tableRef);
</script>

<template>
  <div class="main">
    <el-form
      ref="formRef"
      :inline="true"
      :model="form"
      class="search-form bg-bg_color w-full pl-8 pt-[12px] overflow-auto"
    >
      <el-form-item label="标题：" prop="title">
        <el-input
          v-model="form.title"
          placeholder="请输入标题"
          clearable
          class="w-[180px]!"
        />
      </el-form-item>
      <el-form-item label="类型：" prop="type">
        <el-select v-model="form.type" placeholder="请选择" class="w-[90px]!">
          <el-option label="所有" :value="-1" />
          <el-option label="饭堂" :value="0" />
          <el-option label="教学" :value="1" />
          <el-option label="环境" :value="2" />
          <el-option label="其他" :value="3" />
        </el-select>
      </el-form-item>
      <el-form-item label="日期：" prop="date">
        <el-date-picker
          v-model="form.date"
          type="daterange"
          start-placeholder="开始日期"
          end-placeholder="结束日期"
          value-format="YYYY-MM-DD"
          clearable
          class="w-[240px]!"
        />
      </el-form-item>
      <el-form-item>
        <el-button
          type="primary"
          :icon="useRenderIcon('ri/search-line')"
          :loading="loading"
          @click="onSearch"
        >
          搜索
        </el-button>
        <el-button :icon="useRenderIcon(Refresh)" @click="resetForm(formRef)">
          重置
        </el-button>
      </el-form-item>
    </el-form>

    <PureTableBar title="短建议列表" :columns="columns" @refresh="onSearch">
      <template #buttons>
        <el-button type="primary" @click="exportCurrent()">
          导出当前页建议
        </el-button>
        <el-dropdown>
          <el-button class="ml-3! mt-[2px]!">按AI评分排序</el-button>
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
          :adaptiveConfig="{ offsetBottom: 80 }"
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
          <template #type="{ row }">
            <el-tag v-if="row.type === 0" type="success" class="text-xs">
              饭堂
            </el-tag>
            <el-tag v-else-if="row.type === 1" type="info" class="text-xs">
              教学
            </el-tag>
            <el-tag v-else-if="row.type === 2" type="warning" class="text-xs">
              环境
            </el-tag>
            <el-tag v-else-if="row.type === 3" type="danger" class="text-xs">
              其他
            </el-tag>
          </template>
          <template #score="{ row }">
            <p v-if="row.score === 0" class="text-xs text-gray-500">暂无评分</p>
            <el-rate
              v-else
              :model-value="parseFloat(Number(row.score).toFixed(2))"
              disabled
              allow-half
              :show-score="true"
              class="text-xs"
            />
          </template>
          <template #title="{ row }">
            <el-tooltip
              v-if="row.title && row.title.length > 25"
              :content="row.title"
              placement="top"
              popper-class="notice-title-popper"
              :disabled="!row.title"
            >
              <div
                style="
                  overflow: hidden;
                  text-overflow: ellipsis;
                  white-space: nowrap;
                "
              >
                {{ row.title.slice(0, 25) + "..." }}
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
              {{ row.title }}
            </div>
          </template>
          <template #operation="{ row }">
            <el-button
              class="reset-margin"
              link
              type="primary"
              :size="size"
              :icon="useRenderIcon('ri:eye-line')"
              @click="openDetailDialog('内容详情', row)"
            >
              查看内容
            </el-button>
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
