<template>
  <div class="notices-item">
    <Swiper
      :pagination="{ dynamicBullets: true }"
      :autoplay="{ delay: 2000, disableOnInteraction: false }"
      :loop="true"
    >
      <swiper-slide v-for="i in props.datas.length" :key="i">
        <el-card class="notices-card">
          <div class="notices-content">
            <div class="notices-icon">
              <img src="@/assets/img/notice.svg" />
            </div>
            <div class="notices-text">
              <p class="card-text text-ellipsis">
                {{ props.datas[i - 1].text }}
              </p>
            </div>
            <div class="notices-btn">
              <el-button type="primary" size="small" @click="$emit('toDetail', i - 1)">
                详情
              </el-button>
            </div>
          </div>
        </el-card>
      </swiper-slide>
    </Swiper>
  </div>
</template>

<script setup lang="ts">
import 'swiper/css';
import 'swiper/css/pagination';
import SwiperCore from 'swiper';
import { Swiper, SwiperSlide } from 'swiper/vue';
import { Autoplay, Pagination } from 'swiper/modules';

SwiperCore.use([Autoplay, Pagination]);

const props = defineProps({
  datas: {
    type: Array as () => { text: string; id: number | string }[],
    default: () => [],
  },
});
</script>

<style scoped>
.notices-card {
  margin: 0.6rem 0.6rem 1.8rem 0.6rem;
  border-radius: 1rem;
}

:deep(.el-card__body) {
  padding: 0.8rem 1.2rem;
}

.notices-content {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 4rem;
}

.notices-icon img {
  width: 1.8rem;
  height: 1.8rem;
  margin-right: 0.7rem;
  padding-top: 0.3rem;
}

.notices-text {
  flex: 1;
  font-size: 0.9rem;
  color: #333;
  display: -webkit-box;
  overflow: hidden;
  -webkit-line-clamp: 2;
  line-clamp: 2;
  -webkit-box-orient: vertical;
}

.notices-text p {
  margin: 0;
}

.notices-btn {
  margin-left: 0.4rem;
}
</style>
