<script setup lang="ts">
import { ref } from "vue";
import ReCol from "@/components/ReCol";
import { formRules } from "../utils/rule";
import { FormProps } from "../utils/types";
import { watch } from "node:fs";

const props = withDefaults(defineProps<FormProps>(), {
  formInline: () => ({
    question_id: null,
    title: "添加",
    question: "",
    options: {
      A: "",
      B: "",
      C: "",
      D: ""
    },
    answer: "",
    type: null
  })
});

const ruleFormRef = ref();
const newFormInline = ref(props.formInline);

const isChooseType = ref(
  newFormInline.value ? newFormInline.value.type === 2 : false
);

function convertAnswerText() {
  const cnNumMap: Record<string, string> = {
    零: "0",
    一: "1",
    二: "2",
    三: "3",
    四: "4",
    五: "5",
    六: "6",
    七: "7",
    八: "8",
    九: "9"
  };

  if (newFormInline.value && typeof newFormInline.value.answer === "string") {
    // 转换小写字母为大写
    let answer = newFormInline.value.answer.replace(/[a-z]/g, m =>
      m.toUpperCase()
    );
    // 转换简体中文数字为阿拉伯数字
    answer = answer.replace(/[零一二三四五六七八九]/g, m => cnNumMap[m] || m);
    newFormInline.value.answer = answer;
  }
}

function handleTypeChange(value) {
  isChooseType.value = value === 2;
}

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
      <re-col>
        <el-form-item label="类型" prop="type">
          <el-select
            v-model="newFormInline.type"
            placeholder="请选择"
            @change="handleTypeChange"
          >
            <el-option label="判断题" :value="0" />
            <el-option label="填空题" :value="1" />
            <el-option label="选择题" :value="2" />
          </el-select>
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="题目" prop="question">
          <el-input
            v-model="newFormInline.question"
            placeholder="请输入题目"
            type="textarea"
          />
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30" v-if="isChooseType">
      <re-col :value="12">
        <el-form-item label="A选项">
          <el-input
            v-model="newFormInline.options.A"
            clearable
            placeholder="请输入A选项"
          />
        </el-form-item>
      </re-col>
      <re-col :value="12">
        <el-form-item label="B选项">
          <el-input
            v-model="newFormInline.options.B"
            clearable
            placeholder="请输入B选项"
          />
        </el-form-item>
      </re-col>
      <re-col :value="12">
        <el-form-item label="C选项">
          <el-input
            v-model="newFormInline.options.C"
            clearable
            placeholder="请输入C选项"
          />
        </el-form-item>
      </re-col>
      <re-col :value="12">
        <el-form-item label="D选项">
          <el-input
            v-model="newFormInline.options.D"
            clearable
            placeholder="请输入D选项"
          />
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="答案" prop="answer">
          <el-input
            v-model="newFormInline.answer"
            clearable
            placeholder="请输入答案（判断题用中文“对”或“错”）"
            @change="convertAnswerText"
          />
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="30">
      <re-col>
        <el-form-item label="提示">
          <el-text>
            1. 建议选择题和填空题各10条以上，判断题15条以上<br />
            2.
            判断题需存在不同答案的题目，即不能所有判断题的答案都为“对”或“错”的其中一种<br />
            3. 尽量使用较简单且外校人员了解较少的题目，例如利用校规出题<br />
            4.
            答案的所有字母将自动转换为大写（用户端相同，用于防止答案校验错误）<br />
            5.
            所有汉字数字(简体)将自动转换为阿拉伯数字（用户端相同，用于防止答案校验错误）<br />
          </el-text>
        </el-form-item>
      </re-col>
    </el-row>
  </el-form>
</template>
