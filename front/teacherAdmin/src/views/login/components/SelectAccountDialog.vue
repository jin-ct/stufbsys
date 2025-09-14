<script setup lang="ts">
import { defineProps, defineEmits } from "vue";
const props = defineProps<{
  accounts: Array<{
    name: string;
    grade: string;
    class: string;
    subject: string;
    teacherId: string;
  }>;
}>();
const emit = defineEmits<{
  (e: "select", teacherId: string): void;
}>();
function handleSelect(teacherId: string) {
  emit("select", teacherId);
}
</script>

<template>
  <el-form label-width="80px">
    <el-row>
      <el-col :span="24">
        <el-text>检测到有多个同名账号，请选择您的身份：</el-text>
      </el-col>
    </el-row>
    <el-row>
      <el-col :span="24">
        <div style="display: flex; justify-content: center;">
          <el-table :data="accounts" style="width: 100%; text-align: center;">
            <el-table-column prop="name" label="姓名" align="center" />
            <el-table-column prop="grade" label="年级" align="center" />
            <el-table-column prop="class" label="班级" align="center" />
            <el-table-column prop="subject" label="学科" align="center" />
            <el-table-column label="操作" width="120" align="center">
              <template #default="scope">
          <el-button
            type="primary"
            size="small"
            @click="handleSelect(scope.row.teacherId)"
          >
            选择
          </el-button>
              </template>
            </el-table-column>
          </el-table>
        </div>
      </el-col>
    </el-row>
  </el-form>
</template>
