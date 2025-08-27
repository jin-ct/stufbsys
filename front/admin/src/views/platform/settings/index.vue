<script setup lang="ts">
import { Plus } from "@element-plus/icons-vue";
import { useSettings } from "./utils/hook";
import "./utils/settings.css";

import QuestionIcon from "~icons/ri/question-line";
import DeleteIcon from "~icons/ep/delete";

defineOptions({
  name: "PlatformSettings"
});

const {
  siteForm,
  entryForm,
  aiForm,
  recordForm,
  accountForm,
  isTesting,
  handleLogoChange,
  handleTopImgChange,
  saveSiteSettings,
  saveEntrySettings,
  saveAiSettings,
  testApi,
  saveRecordSettings,
  saveAccountSettings
} = useSettings();
</script>

<template>
  <div class="settings-page settings-grid">
    <div class="settings-cell">
      <el-card class="box-card" shadow="hover">
        <template #header>
          <span>网站设置</span>
        </template>
        <el-form :model="siteForm" label-width="110px" class="settings-form">
          <el-form-item label="学校名称">
            <el-input
              v-model="siteForm.schoolName"
              placeholder="请输入学校名称"
            />
          </el-form-item>
          <el-form-item label="网站标题">
            <el-input
              v-model="siteForm.homeTitle"
              placeholder="请输入网站标题"
            />
          </el-form-item>
          <el-form-item label="网站Logo">
            <el-upload
              class="avatar-uploader"
              accept="image/*"
              :limit="1"
              :show-file-list="false"
              :auto-upload="false"
              :on-change="handleLogoChange"
            >
              <img
                v-if="siteForm.logoUrl"
                :src="siteForm.logoUrl"
                class="logo-img"
              />
              <el-icon v-else><Plus /></el-icon>
            </el-upload>
            <DeleteIcon
              v-if="siteForm.logoUrl"
              style="margin-left: 8px; color: darkslategrey; cursor: pointer"
              @click="siteForm.logoUrl = ''"
            />
          </el-form-item>
          <el-form-item label="首页顶部图片">
            <el-upload
              class="avatar-uploader"
              :limit="1"
              :show-file-list="false"
              :auto-upload="false"
              :on-change="handleTopImgChange"
            >
              <img
                v-if="siteForm.topImgUrl"
                :src="siteForm.topImgUrl"
                class="top-img"
              />
              <el-icon v-else><Plus /></el-icon>
            </el-upload>
            <DeleteIcon
              v-if="siteForm.topImgUrl"
              style="margin-left: 8px; color: darkslategrey; cursor: pointer"
              @click="siteForm.topImgUrl = ''"
            />
          </el-form-item>
          <el-form-item label="首页图片标题">
            <el-switch
              v-model="siteForm.showHomeTitle"
              active-text="显示"
              inactive-text="隐藏"
            />
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="saveSiteSettings"
              >保存设置</el-button
            >
          </el-form-item>
        </el-form>
      </el-card>
    </div>
    <div class="settings-cell">
      <el-card class="box-card" shadow="hover">
        <template #header>
          <span>入口开关</span>
        </template>
        <el-form :model="entryForm" label-width="120px" class="settings-form">
          <el-form-item label="“饭堂”功能入口">
            <el-switch
              v-model="entryForm.enableCanteen"
              active-text="开启"
              inactive-text="关闭"
              @change="saveEntrySettings"
            />
          </el-form-item>
          <el-form-item label="“加菜”功能入口">
            <el-switch
              v-model="entryForm.enableAddDish"
              active-text="开启"
              inactive-text="关闭"
              @change="saveEntrySettings"
              :disabled="!entryForm.enableCanteen"
            />
          </el-form-item>
        </el-form>
      </el-card>
    </div>
    <div class="settings-cell">
      <el-card class="box-card" shadow="hover">
        <template #header>
          <span style="display: inline-flex; align-items: center; gap: 6px">
            AI调用设置
            <el-tooltip
              content="使用OpenAI调用方式，适用于大部分主流AI模型，如：DeepSeek、通义千问、智谱清言等；如不了解，请在网络上搜索AI名称+参数(如：Deepseek api_url)"
              placement="top"
            >
              <QuestionIcon
                style="
                  color: black;
                  font-size: 18px;
                  cursor: pointer;
                  vertical-align: middle;
                "
              />
            </el-tooltip>
          </span>
        </template>
        <el-form :model="aiForm" label-width="100px" class="settings-form">
          <el-form-item label="API链接">
            <el-input
              v-model="aiForm.aiApiUrl"
              placeholder="请输入PI调用链接(api_url)"
            />
          </el-form-item>
          <el-form-item label="API密钥">
            <el-input
              v-model="aiForm.aiApiKey"
              placeholder="请输入API密钥(api_key), 不显示"
            />
          </el-form-item>
          <el-form-item label="AI模型名称">
            <el-input
              v-model="aiForm.aiModelName"
              placeholder="对应AI平台提供的模型名称"
            />
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="saveAiSettings"
              >保存设置</el-button
            >
            <el-button @click="testApi" :loading="isTesting"
              >测试调用</el-button
            >
          </el-form-item>
        </el-form>
      </el-card>
    </div>
    <div class="settings-cell">
      <el-card class="box-card" shadow="hover">
        <template #header>
          <span style="display: inline-flex; align-items: center; gap: 6px">
            备案设置
            <el-tooltip
              content="已链接到对应平台，非大陆内服务器（如香港地区云服务器）不用填写，备案流程请参考网络搜索结果"
              placement="top"
            >
              <QuestionIcon
                style="
                  color: black;
                  font-size: 18px;
                  cursor: pointer;
                  vertical-align: middle;
                "
              />
            </el-tooltip>
          </span>
        </template>
        <el-form :model="recordForm" label-width="120px" class="settings-form">
          <el-form-item label="ICP备案号">
            <el-input
              v-model="recordForm.icpNum"
              placeholder="请输入ICP备案号"
            />
          </el-form-item>
          <el-form-item label="公安联网备案号">
            <el-input
              v-model="recordForm.publicSecurityNum"
              placeholder="请输入公安联网备案号"
            />
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="saveRecordSettings"
              >保存设置</el-button
            >
          </el-form-item>
        </el-form>
      </el-card>
    </div>
    <div class="settings-cell">
      <el-card class="box-card" shadow="hover">
        <template #header>
          <span>账号设置</span>
        </template>
        <el-form :model="accountForm" label-width="60px" class="settings-form">
          <el-form-item label="账号">
            <el-select
              v-model="accountForm.account"
              placeholder="请选择账号"
              style="width: 100%"
            >
              <el-option label="最高管理员" :value="0" />
              <el-option label="次级管理员" :value="1" />
              <el-option label="建议管理员" :value="2" />
              <el-option label="饭堂管理员" :value="3" />
            </el-select>
          </el-form-item>
          <el-form-item label="密码">
            <el-input
              v-model="accountForm.password"
              type="password"
              placeholder="请输入新密码（仅修改选择的账号）"
              :show-password="true"
            />
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="saveAccountSettings"
              >保存设置</el-button
            >
          </el-form-item>
        </el-form>
      </el-card>
    </div>
  </div>
</template>
