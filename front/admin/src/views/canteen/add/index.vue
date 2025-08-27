<script setup lang="ts">
import { ref } from "vue";
import { useAddDish } from "./utils/hook";
import { PureTableBar } from "@/components/RePureTableBar";
import { useRenderIcon } from "@/components/ReIcon/src/hooks";

import Delete from "~icons/ep/delete";
import Refresh from "~icons/ep/refresh";
import Check from "~icons/ep/check";

defineOptions({
  name: "AddDish"
});

const formRef = ref();
const tableRef = ref();

const {
  form,
  loading,
  columns,
  dataList,
  selectedNum,
  pagination,
  onSearch,
  resetForm,
  onbatchDel,
  handleDelete,
  handleSizeChange,
  onSelectionCancel,
  handleCurrentChange,
  handleSelectionChange,
  sortByScoreAsc,
  sortByScoreDesc,
  sortByVotesAsc,
  sortByVotesDesc,
  approve,
  exportCurrent
} = useAddDish(tableRef);
</script>

<template>
  <div class="main">
    <el-form
      ref="formRef"
      :inline="true"
      :model="form"
      class="search-form bg-bg_color w-full pl-8 pt-[12px] overflow-auto"
    >
      <el-form-item label="名称：" prop="dishname">
        <el-input
          v-model="form.dishname"
          placeholder="请输入名称"
          clearable
          class="w-[180px]!"
        />
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

    <PureTableBar title="菜品列表" :columns="columns" @refresh="onSearch">
      <template #buttons>
        <el-button type="primary" @click="exportCurrent()">
          导出当前页菜品
        </el-button>
        <el-dropdown>
          <el-button class="ml-3! mt-[2px]!">排序</el-button>
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item>
                <el-button
                  link
                  type="primary"
                  :icon="useRenderIcon('ep:sort-down')"
                  @click="sortByScoreAsc()"
                >
                  按AI评分低分在前
                </el-button>
              </el-dropdown-item>
              <el-dropdown-item>
                <el-button
                  link
                  type="primary"
                  :icon="useRenderIcon('ep:sort-up')"
                  @click="sortByScoreDesc()"
                >
                  按AI评分高分在前
                </el-button>
              </el-dropdown-item>
              <el-dropdown-item>
                <el-button
                  link
                  type="primary"
                  :icon="useRenderIcon('ep:sort-up')"
                  @click="sortByVotesDesc()"
                >
                  投票数高在前
                </el-button>
              </el-dropdown-item>
              <el-dropdown-item>
                <el-button
                  link
                  type="primary"
                  :icon="useRenderIcon('ep:sort-down')"
                  @click="sortByVotesAsc()"
                >
                  投票数低在前
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
          <template #status="{ row }">
            <el-tag :type="row.states === 1 ? 'success' : 'danger'">
              {{ row.states === 1 ? "公开投票" : "已申请" }}
            </el-tag>
          </template>
          <template #score="{ row }">
            <p v-if="row.score === 0" class="text-xs text-gray-500">暂无评分</p>
            <el-rate
              v-else
              :model-value="parseFloat(Number(row.score).toFixed(2))"
              disabled
              allow-half
              size="small"
              :show-score="true"
              class="text-xs"
            />
          </template>
          <template #description="{ row }">
            <el-tooltip
              v-if="row.description && row.description.length > 25"
              :content="row.description"
              placement="top"
              popper-class="notice-title-popper"
              :disabled="!row.description"
            >
              <div
                style="
                  overflow: hidden;
                  text-overflow: ellipsis;
                  white-space: nowrap;
                "
              >
                {{ row.description.slice(0, 25) + "..." }}
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
              {{ row.description }}
            </div>
          </template>
          <template #operation="{ row }">
            <el-popconfirm
              :title="`是否确认通过菜品ID为${row.id}的这条数据`"
              @confirm="approve(row)"
            >
              <template #reference>
                <el-button
                  class="reset-margin"
                  link
                  type="primary"
                  :size="size"
                  :icon="useRenderIcon(Check)"
                >
                  通过
                </el-button>
              </template>
            </el-popconfirm>
            <el-popconfirm
              :title="`是否确认删除菜品ID为${row.id}的这条数据`"
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
