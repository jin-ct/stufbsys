<script setup lang="ts">
import { ref } from "vue";
import ReCol from "@/components/ReCol";
import { formRules } from "../utils/rule";
import { FormProps } from "../utils/types";

const props = withDefaults(defineProps<FormProps>(), {
  formInline: () => ({
    title: "添加",
    notice_id: 0,
    publisher: "",
    show_text: "",
    detail: "",
    is_show: true,
    date: ""
  })
});

const ruleFormRef = ref();
const newFormInline = ref(props.formInline);

function getRef() {
  return ruleFormRef.value;
}

defineExpose({ getRef });
</script>

<template>
  <el-form
    ref="ruleFormRef"
    :model="newFormInline"
    :rules="formRules"
    label-width="82px"
  >
    <el-row :gutter="30">
      <re-col :value="12" :xs="24" :sm="24">
        <el-form-item label="发布人" prop="publisher">
          <el-input
            v-model="newFormInline.publisher"
            clearable
            placeholder="请输入发布人"
          />
        </el-form-item>
      </re-col>
      <re-col :value="12" :xs="24" :sm="24">
        <el-form-item label="日期" prop="date">
          <el-date-picker
            v-model="newFormInline.date"
            type="date"
            value-format="YYYY-MM-DD"
            placeholder="选择日期"
            clearable
          />
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="展示文本" prop="show_text">
          <el-input
            v-model="newFormInline.show_text"
            clearable
            placeholder="请输入首页展示文本"
          />
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="详情" prop="detail">
          <el-input
            v-model="newFormInline.detail"
            placeholder="请输入详情（留空则与首页展示文本相同）"
            type="textarea"
          />
        </el-form-item>
      </re-col>
    </el-row>
  </el-form>
</template>
