<script setup lang="ts">
import { ref } from "vue";
import { useDish } from "./utils/hook";
import { PureTableBar } from "@/components/RePureTableBar";
import { useRenderIcon } from "@/components/ReIcon/src/hooks";

import Refresh from "~icons/ep/refresh";

defineOptions({
  name: "Evaluate"
});

const formRef = ref();
const tableRef = ref();

const {
  form,
  loading,
  columns,
  dataList,
  pagination,
  onSearch,
  resetForm,
  handleSizeChange,
  handleCurrentChange,
  handleSelectionChange,
  openDetailDialog,
  exportAll,
  sortByScoreAsc,
  sortByScoreDesc
} = useDish(tableRef);
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

    <PureTableBar title="菜品评价" :columns="columns" @refresh="onSearch">
      <template #buttons>
        <el-button type="primary" @click="exportAll()">
          导出全部评价
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
        <pure-table
          ref="tableRef"
          row-key="dish_id"
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
          <template #score="{ row }">
            <p v-if="row.avg_score === 0" class="text-xs text-gray-500">
              暂无评分
            </p>
            <el-rate
              v-else
              :model-value="parseFloat(Number(row.avg_score).toFixed(2))"
              disabled
              allow-half
              :show-score="true"
              class="text-xs"
            />
          </template>
          <template #suggestion="{ row }">
            <el-tooltip
              v-if="row.lastest_evaluate && row.lastest_evaluate.length > 30"
              :content="row.lastest_evaluate"
              placement="top"
              popper-class="notice-title-popper"
              :disabled="!row.lastest_evaluate"
            >
              <div
                style="
                  overflow: hidden;
                  text-overflow: ellipsis;
                  white-space: nowrap;
                "
              >
                {{ row.lastest_evaluate.slice(0, 30) + "..." }}
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
              {{ row.lastest_evaluate }}
            </div>
          </template>
          <template #operation="{ row }">
            <el-button
              class="reset-margin"
              link
              type="primary"
              :size="size"
              :icon="useRenderIcon('ri:eye-line')"
              @click="openDetailDialog('评价详情', row)"
            >
              查看评价详情
            </el-button>
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
