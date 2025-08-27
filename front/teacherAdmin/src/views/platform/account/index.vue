<script setup lang="ts">
import { useSettings } from "./utils/hook";
import "./utils/settings.css";

defineOptions({
  name: "accountSettings"
});

const { form, onSave } = useSettings();
</script>

<template>
  <div class="settings-page">
    <el-card class="box-card" shadow="hover">
      <template #header>
        <span>账号设置</span>
      </template>
      <el-form :model="form" label-width="80px" class="settings-form">
        <el-form-item label="邮箱" class="w-[95%]!">
          <el-input v-model="form.email" placeholder="请输入邮箱（选填）" />
          <p style="color: gray; font-size: 13px">有新建议时通过邮箱发送通知</p>
        </el-form-item>
        <el-form-item label="登录密码" class="w-[95%]!">
          <el-input v-model="form.password" placeholder="请设置新的登录密码" />
          <p style="color: gray; font-size: 13px">忘记密码请联系学校管理员</p>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="onSave">保存设置</el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>
