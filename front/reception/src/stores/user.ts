import { defineStore } from 'pinia';
import { ref } from 'vue';

export const useUserStore = defineStore('user', () => {
  const visitingMode = ref(false);
  const needRegister = ref(false);
  const needVerify = ref(false);
  const tempToken = ref('');

  return {
    visitingMode,
    needRegister,
    needVerify,
    tempToken,
  };
});
