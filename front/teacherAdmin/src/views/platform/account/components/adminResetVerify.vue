<script setup lang="ts">
import { ref } from "vue";
import ReCol from "@/components/ReCol";

const props = defineProps<{
  formInline: {
    verification_code: string;
  };
}>();

const newFormInline = ref(props.formInline);
</script>

<template>
  <el-form ref="ruleFormRef" :model="newFormInline" label-width="82px">
    <el-row :gutter="60">
      <re-col>
        <el-form-item label="提示">
          <el-text>该操作由学校端后台发起，如不了解情况，请点击"取消"以忽略此弹窗</el-text>
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="60">
      <re-col>
        <el-form-item label="验证码" prop="verification_code">
          <el-input
            v-model="newFormInline.verification_code"
            clearable
            placeholder="请输入验证码"
          />
        </el-form-item>
      </re-col>
    </el-row>
  </el-form>
</template>
