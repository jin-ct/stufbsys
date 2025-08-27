<template>
  <div class="main-container">
    <div class="content">
      <el-row>
        <el-col :span="24">
          <Title
            :school="settings.school_name"
            :bannerUrl="settings.home_title_image"
            :showText="settings.is_show_home_title_image"
          />
        </el-col>
      </el-row>
      <el-row>
        <el-col :span="24">
          <Notices :datas="notices" :key="notices.length" @toDetail="showNoticeDetail" />
        </el-col>
      </el-row>
      <el-row>
        <el-col :span="24">
          <FunctionBtn
            v-if="settings.is_show_canteen_entrance"
            text="饭堂菜品评价"
            @click="router.push({ name: 'canteen' })"
          />
        </el-col>
      </el-row>
      <el-row>
        <el-col :span="24">
          <FunctionBtn text="对老师提建议" @click="router.push({ name: 'for-teacher' })" />
        </el-col>
      </el-row>
      <el-row>
        <el-col :span="24">
          <FunctionBtn text="对学校提建议" @click="router.push({ name: 'for-school' })" />
        </el-col>
      </el-row>
    </div>
    <el-footer class="fixed-footer">
      <div class="link">
        学生反馈系统：
        <el-link
          type="primary"
          href="https://stufbsys.creatisky.cn"
          target="_blank"
          style="font-weight: 430; font-size: 0.8rem"
        >
          项目详情
        </el-link>
      </div>
      <div class="link" v-if="settings.icp_num">
        <el-link
          type="info"
          href="https://beian.miit.gov.cn/"
          target="_blank"
          style="font-weight: 420; font-size: 0.8rem"
        >
          {{ settings.icp_num }}
        </el-link>
      </div>
      <div class="link" v-if="settings.beian_mps">
        <el-link
          type="info"
          :href="mps_src"
          target="_blank"
          style="font-weight: 420; font-size: 0.8rem"
        >
          {{ settings.beian_mps }}
        </el-link>
      </div>
    </el-footer>
    <Dialog v-model="noticeDetailVisible" title="公告详情">
      <NoticesDetail :notice="noticeDetail" />
    </Dialog>
    <Dialog v-model="examDialogVisible" title="入站考试" :isShowClose="false">
      <Exam />
    </Dialog>
    <Dialog v-model="verificationDialogVisible" title="人机验证" :isShowClose="false">
      <Verification />
    </Dialog>
  </div>
</template>

<script setup lang="ts">
import router from '@/router';
import Title from './components/title.vue';
import Notices from './components/notices.vue';
import FunctionBtn from '../../components/function-btn.vue';
import Dialog from '@/components/dialog.vue';
import NoticesDetail from './components/notices-detail.vue';
import Exam from './components/exam.vue';
import Verification from './components/verification.vue';
import { onMounted, reactive, ref, watch } from 'vue';
import { useUserStore } from '@/stores/user';
import { useSettings } from '@/hooks/useSettings';
import { getNotices } from '@/api/notices';

const { settings, updateSettings } = useSettings();
const mps_src = ref('http://www.beian.gov.cn/portal/registerSystemInfo?recordcode=');
mps_src.value += (settings.beian_mps.match(/\d+/g) || []).join('');

const notices = ref([]);
const noticeDetail = ref();
const fetchNotices = async () => {
  const res = await getNotices();
  if (res) {
    notices.value = res.data.data.notices;
  }
};
const showNoticeDetail = (id: number) => {
  noticeDetail.value = notices.value[id];
  noticeDetailVisible.value = true;
};

onMounted(() => {
  fetchNotices();
});

const userStore = useUserStore();
const examDialogVisible = ref(userStore.needRegister);
watch(
  () => userStore.needRegister,
  (newVal) => {
    examDialogVisible.value = newVal;
    if (!examDialogVisible.value) {
      updateSettings();
      fetchNotices();
    }
  },
);
const verificationDialogVisible = ref(userStore.needVerify);
watch(
  () => userStore.needVerify,
  (newVal) => {
    verificationDialogVisible.value = newVal;
    if (!verificationDialogVisible.value && !examDialogVisible.value) {
      updateSettings();
      fetchNotices();
    }
  },
);
const noticeDetailVisible = ref(false);
</script>

<style scoped>
@media screen and (max-width: 767px) {
  .main-container {
    width: 100%;
    height: 100vh;
    background-color: #f2f3f5;
  }

  .fixed-footer {
    flex-direction: column;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  .fixed-footer .link {
    width: 100%;
    justify-content: center;
    display: flex;
  }
}

@media screen and (min-width: 768px) {
  .main-container {
    background-color: #f2f3f5;
    width: 100%;
    height: 100vh;
    display: flex;
  }

  .main-container .content {
    width: 460px;
    height: 480px;
    margin: auto;
    background-color: #fafafa;
    border-radius: 18px;
    box-shadow: 0 4px 24px rgba(0, 0, 0, 0.04);
    overflow: hidden;
  }

  .fixed-footer .link {
    margin: 0 0.35rem;
  }
}

.fixed-footer .link {
  text-align: center;
  font-size: 0.8rem;
  color: rgb(115.2, 117.6, 122.4);
  align-items: center;
  display: inline-flex;
  justify-content: center;
}

.fixed-footer {
  position: fixed;
  left: 0;
  bottom: 0;
  width: 100vw;
  text-align: center;
  padding: 0.8rem 0.8rem;
  z-index: 100;
  height: auto;
  align-items: center;
  display: inline-flex;
  justify-content: center;
}
</style>
