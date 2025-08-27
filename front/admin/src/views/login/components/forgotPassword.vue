<script setup lang="ts">
import { onMounted, ref } from "vue";
import ReCol from "@/components/ReCol";
import { loginRules } from "../utils/rule";
import { getAdminResetVerificationInf } from "@/api/user";

const props = defineProps<{
  formInline: {
    password: string;
    verification_code: string;
  };
}>();

const newFormInline = ref(props.formInline);
const verificationCount = ref();

onMounted(() => {
  getAdminResetVerificationInf().then(res => {
    if (res.success) {
      newFormInline.value.verification_code = res.data.code;
      verificationCount.value = res.data.teachers_count;
    }
  });
});
</script>

<template>
  <el-form
    v-if="newFormInline.verification_code === ''"
    ref="ruleFormRef"
    :model="newFormInline"
    :rules="loginRules"
    label-width="82px"
  >
    <el-row :gutter="60">
      <re-col>
        <el-form-item label="普通管理员">
          <el-text>请在最高管理员账号 ( 平台设置 > 网站设置 > 账号设置 ) 中修改</el-text>
        </el-form-item>
      </re-col>
    </el-row>
    <el-row :gutter="60">
      <re-col>
        <el-form-item label="最高管理员" prop="password">
          <el-input
            v-model="newFormInline.password"
            clearable
            placeholder="请设置新密码"
          />
        </el-form-item>
      </re-col>
    </el-row>
  </el-form>
  <div
    v-else
    style="
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
    "
  >
    <el-text style="font-size: 20px">
      验证码: {{ newFormInline.verification_code }}
    </el-text>
    <el-text style="font-size: 16px; margin-top: 10px">
      请在一小时内邀请3位教师登录教师端后台
    </el-text>
    <el-text style="font-size: 16px">
      在教师端后台 > 平台设置 > 账号设置页面弹出的窗口中输入验证码完成验证
    </el-text>
    <el-text style="font-size: 14px; margin-top: 10px">
      已有{{ verificationCount }}位教师完成验证
    </el-text>
  </div>
</template>
