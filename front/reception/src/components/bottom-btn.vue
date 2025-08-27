<template>
  <div class="bottom-btn">
    <el-button class="btn" :type="props.btn1['btn-type']" round @click="$emit('btn1Click')">
      {{ props.btn1['btn-text'] }}
    </el-button>
    <el-button
      v-if="btnNum === 2"
      class="btn"
      :type="props.btn2['btn-type']"
      round
      @click="$emit('btn2Click')"
    >
      {{ props.btn2['btn-text'] }}
    </el-button>
  </div>
</template>

<script setup lang="ts">
import type { PropType } from 'vue';

interface BtnConfig {
  'btn-text': String;
  'btn-type'?: String;
}

const props = defineProps({
  btnNum: {
    type: Number,
    default: 1,
  },
  btn1: {
    type: Object as PropType<BtnConfig>,
    default: () => ({
      'btn-text': '按钮1',
      'btn-type': '',
    }),
  },
  btn2: {
    type: Object as PropType<BtnConfig>,
    default: () => ({
      'btn-text': '按钮2',
      'btn-type': '',
    }),
  },
});
</script>

<style scoped>
.bottom-btn {
  position: absolute;
  left: 0;
  right: 0;
  bottom: 0;
  display: flex;
  justify-content: center;
  padding: 1rem;
  z-index: 100;
  height: 2.8rem;
}

.bottom-btn .btn {
  height: 2.4rem;
  margin: 0 0.8rem;
  padding: 0 2.4rem;
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.06);
}
</style>
