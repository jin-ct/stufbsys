<script setup lang="ts">
import { getConfig } from "@/config";
import { ref } from "vue";
import { getFooterSaying } from "@/api/platform";

const TITLE = getConfig("Title");

const sayingText = ref("");

getFooterSaying().then(res => {
  if (res.success) {
    sayingText.value = res.data.saying;
  }
});
</script>

<template>
  <footer
    class="layout-footer text-[rgba(0,0,0,0.6)] dark:text-[rgba(220,220,242,0.8)]"
  >
    {{ sayingText }}
  </footer>
</template>

<style lang="scss" scoped>
.layout-footer {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 100%;
  padding: 0 0 8px;
  font-size: 14px;
}
</style>
