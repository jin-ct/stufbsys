<template>
  <div class="add-suggestion">
    <div class="title-box">
      <el-input
        v-model="title"
        placeholder="请输入标题"
        :maxlength="40"
        show-word-limit
        :disabled="visitingMode"
      />
    </div>
    <div class="content-box">
      <el-input
        :disabled="visitingMode"
        v-model="content"
        type="textarea"
        placeholder="请输入建议内容"
        :rows="5"
        :maxlength="200"
        show-word-limit
        v-if="!isLong"
      />
      <div class="content-long" v-if="isLong">
        <Toolbar
          style="border-bottom: 1px solid #ccc"
          :editor="editorRef"
          :defaultConfig="toolbarConfig"
          mode="default"
        />
        <Editor
          :disabled="visitingMode"
          style="height: 500px; overflow-y: hidden"
          v-model="content"
          :defaultConfig="editorConfig"
          mode="default"
          @onCreated="handleCreated"
        />
      </div>
    </div>
    <div class="type-box">
      <el-select
        v-model="typeID"
        placeholder="请选择建议类型"
        style="width: 100%"
        :disabled="visitingMode"
      >
        <el-option label="饭堂建议" :value="0"></el-option>
        <el-option label="教学建议" :value="1"></el-option>
        <el-option label="环境建议" :value="2"></el-option>
        <el-option label="其他建议" :value="3"></el-option>
      </el-select>
    </div>
    <div class="info-box" v-if="!isAnonymous">
      <el-input v-model="name" placeholder="姓名" :maxlength="8" show-word-limit />
      <el-input v-model="className" placeholder="班级" :maxlength="8" show-word-limit />
      <el-input v-model="grade" placeholder="年级" :maxlength="8" show-word-limit />
    </div>
    <div class="setting-box">
      <span>设置：</span>
      <div>
        <el-checkbox label="公开" v-model="isPublic" :disabled="visitingMode">公开</el-checkbox>
        <el-checkbox label="匿名" v-model="isAnonymous" :disabled="visitingMode">匿名</el-checkbox>
      </div>
    </div>
    <div class="submit-box">
      <el-button v-if="!visitingMode" type="primary" @click="handleSubmit" :loading="isLoading"
        >提交建议</el-button
      >
      <el-button v-else type="primary" @click="toHome">去完成入站考试</el-button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { shallowRef, onBeforeUnmount } from 'vue';
import { ElMessage } from 'element-plus';
import '@wangeditor/editor/dist/css/style.css';
import { Editor, Toolbar } from '@wangeditor/editor-for-vue';
import { ref } from 'vue';
import { useUserStore } from '@/stores/user';
import router from '@/router';

const visitingMode = ref(useUserStore().visitingMode);
if (visitingMode.value) {
  ElMessage.warning('当前为访客模式');
}

const toHome = () => {
  useUserStore().needRegister = true;
  router.push('/');
};

const emit = defineEmits(['submit']);

const { isLong } = defineProps({
  isLong: {
    type: Boolean,
    default: false,
  },
});

const title = defineModel<string>('title', { default: '' });
const content = defineModel<string>('content', { default: '' });
const typeID = defineModel<number>('typeID', { default: null });
const name = defineModel<string>('name', { default: '' });
const className = defineModel<string>('className', { default: '' });
const grade = defineModel<string>('grade', { default: '' });
const isPublic = defineModel<boolean>('public', { default: true });
const isAnonymous = defineModel<boolean>('anonymous', { default: true });

const isLoading = defineModel<boolean>('isLoading', { default: false });

const handleSubmit = () => {
  const formData = {
    title: title.value,
    content: content.value,
    typeID: typeID.value,
    name: name.value,
    className: className.value,
    grade: grade.value,
    isPublic: isPublic.value,
    isAnonymous: isAnonymous.value,
  };
  console.log(content);
  const isContentEmpty = isLong ? editorRef.value.isEmpty() : content.value.trim() === '';
  if (formData.title.trim() === '' || isContentEmpty || formData.typeID === null) {
    return ElMessage.warning('请填写必要字段');
  }
  if (!formData.isAnonymous && !formData.name && !formData.className && !formData.grade) {
    return ElMessage.warning('请填写个人信息');
  }
  emit('submit', formData);
};

const editorRef = shallowRef();
const toolbarConfig = {
  toolbarKeys: [
    'bold',
    'underline',
    'justifyLeft',
    'justifyCenter',
    'justifyRight',
    'color',
    'bgColor',
    {
      key: 'group-link',
      title: '链接',
      menuKeys: ['insertLink', 'editLink', 'unLink', 'viewLink'],
    },
    {
      key: 'group-tools',
      title: '工具',
      menuKeys: ['clearStyle', 'divider', 'emotion'],
    },
    'headerSelect',
  ],
};
const editorConfig = {
  placeholder: '请在此区域进行建议撰写并完成排版',
};
onBeforeUnmount(() => {
  const editor = editorRef.value;
  if (editor == null) return;
  editor.destroy();
});
const handleCreated = (editor: any) => {
  editorRef.value = editor;
};
</script>

<style scoped>
.title-box {
  margin: 0.6rem 0;
}

.content-box {
  margin-bottom: 0.5rem;
}

.submit-box {
  display: flex;
  justify-content: center;
  margin-top: 0.5rem;
}

.info-box {
  margin: 0.5rem 0;
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.setting-box {
  margin: 0.5rem 0;
  display: flex;
  align-items: center;
}

.content-long {
  margin-top: 0.5rem;
  padding: 0.3rem;
  border-radius: 0.2rem;
  border: rgb(221.7, 222.6, 224.4) 1px solid;
  max-height: 22rem;
  overflow-y: auto;
  position: relative;
}
</style>
