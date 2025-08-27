<script setup lang="ts">
import User from "~icons/ep/user";
import School from "~icons/ep/school";
import Class from "~icons/ep/office-building";

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
        <el-col
          v-if="props.row.name"
          :span="3"
          style="display: flex; align-items: center"
        >
          <span
            style="display: inline-flex; align-items: center; font-size: 15px"
          >
            <User style="color: black; margin-right: 6px" />
            <span>{{ props.row.name }}</span>
          </span>
        </el-col>
        <el-col
          v-if="props.row.grade"
          :span="3"
          style="display: flex; align-items: center"
        >
          <span
            style="display: inline-flex; align-items: center; font-size: 15px"
          >
            <School style="color: black; margin-right: 6px" />
            <span>{{ props.row.grade }}</span>
          </span>
        </el-col>
        <el-col
          v-if="props.row.class"
          :span="3"
          style="display: flex; align-items: center"
        >
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
          <el-input
            v-model="props.row.content"
            type="textarea"
            :readonly="true"
            :rows="5"
            placeholder="暂无内容"
            class="w-full"
          />
        </el-col>
      </el-row>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.main-content {
  margin: 5px 20px !important;
}
</style>
