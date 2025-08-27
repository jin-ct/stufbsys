import { onMounted, ref, h } from "vue";
import { addDialog } from "@/components/ReDialog";
import { message } from "@/utils/message";
import AdminResetVerify from "../components/adminResetVerify.vue";
import {
  saveAccountSettings,
  getSettings,
  verifyAdminReset
} from "@/api/platform";

export function useSettings() {
  const form = ref({
    email: "",
    password: ""
  });

  onMounted(async () => {
    const { data } = await getSettings({});
    form.value.email = data.email;
    if (data.isReset) {
      openAdminResetVerifyDialog();
    }
  });

  function openAdminResetVerifyDialog() {
    addDialog({
      title: "验证管理员密码重置",
      width: "42%",
      props: {
        formInline: {
          verification_code: ""
        }
      },
      draggable: true,
      fullscreenIcon: true,
      closeOnClickModal: false,
      contentRenderer: () => h(AdminResetVerify, { formInline: null }),
      beforeSure: async (done, { options }) => {
        const curData = options.props.formInline;
        if (curData.verification_code == "") return;
        const { success, data } = await verifyAdminReset({
          verification_code: curData.verification_code
        });
        if (success) {
          options.props.formInline.verification_code = data.verification_code;
          message(data.message, { type: "success" });
          done();
        } else {
          message(data.message, { type: "error" });
        }
      }
    });
  }

  async function onSave() {
    if (
      form.value.password &&
      (form.value.password.length < 8 || form.value.password.length > 22)
    ) {
      message("长度必须在8到22个字符之间", { type: "warning" });
      return;
    }
    const { success } = await saveAccountSettings({
      ...form.value
    });
    if (success) {
      message("保存成功", { type: "success" });
    } else {
      message("保存失败", { type: "error" });
    }
  }
  return {
    form,
    onSave
  };
}
