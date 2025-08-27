<script setup lang="ts">
import User from "~icons/ep/user";
import School from "~icons/ep/school";
import Class from "~icons/ep/office-building";
import { ref } from "vue";

defineOptions({
  name: "EvaluateDetail"
});

const props = defineProps({
  row: {
    type: Object,
    required: true
  }
});
</script>

<template>
  <div class="main">
    <div class="main-content">
      <el-row class="mb-4" style="margin-bottom: 5px">
        <el-col v-if="props.row.name" :span="3">
          <span
            style="display: inline-flex; align-items: center; font-size: 15px"
          >
            <User style="color: black; margin-right: 6px" />
            <span>{{ props.row.name }}</span>
          </span>
        </el-col>
        <el-col v-if="props.row.grade" :span="3">
          <span
            style="display: inline-flex; align-items: center; font-size: 15px"
          >
            <School style="color: black; margin-right: 6px" />
            <span>{{ props.row.grade }}</span>
          </span>
        </el-col>
        <el-col v-if="props.row.class" :span="3">
          <span
            style="display: inline-flex; align-items: center; font-size: 15px"
          >
            <Class style="color: black; margin-right: 6px" />
            <span>{{ props.row.class }}</span>
          </span>
        </el-col>
      </el-row>
      <el-row class="mb-4">
        <el-col>
          <div v-html="props.row.content" class="content"></div>
        </el-col>
      </el-row>
    </div>
  </div>
</template>

<style scoped>
.main-content {
  margin: 5px 20px !important;
}

.content {
  border-radius: 4px;
  border: 1px solid var(--el-border-color);
  padding: 10px;
}
</style>
