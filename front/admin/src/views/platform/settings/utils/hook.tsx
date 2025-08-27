import { onMounted, ref } from "vue";
import {
  saveSettings,
  getSettings,
  uploadLogo,
  uploadHomeImage,
  setAdminPassword
} from "@/api/platform";
import { message } from "@/utils/message";

export function useSettings() {
  // 网站设置表单
  const siteForm = ref({
    schoolName: "",
    homeTitle: "",
    logoUrl: "",
    topImgUrl: "",
    showHomeTitle: true
  });
  // 入口开关表单
  const entryForm = ref({
    enableCanteen: true,
    enableAddDish: true
  });
  // AI调用设置表单
  const isTesting = ref(false);
  const aiForm = ref({
    aiApiUrl: "",
    aiApiKey: "",
    aiModelName: ""
  });

  // 备案设置表单
  const recordForm = ref({
    icpNum: "",
    publicSecurityNum: ""
  });

  // 账号设置表单
  const accountForm = ref({
    account: 0,
    password: ""
  });

  onMounted(() => {
    getSettings().then((res: any) => {
      if (res.success) {
        const settings = res.data;
        siteForm.value = {
          schoolName: settings.school_name,
          homeTitle: settings.web_title,
          logoUrl: settings.web_logo,
          topImgUrl: settings.home_title_image,
          showHomeTitle: settings.is_show_home_title_image === "1"
        };
        entryForm.value = {
          enableCanteen: settings.is_show_canteen_entrance === "1",
          enableAddDish: settings.is_show_dish_add_entrance === "1"
        };
        aiForm.value = {
          aiApiUrl: settings.ai_api_link,
          aiApiKey: "",
          aiModelName: settings.ai_model_name
        };
        recordForm.value = {
          icpNum: settings.icp_num,
          publicSecurityNum: settings.beian_mps
        };
      }
    });
  });

  function handleLogoChange(file: any) {
    const formData = new FormData();
    formData.append("file", file.raw);
    console.log(formData);
    uploadLogo(formData)
      .then((res: any) => {
        siteForm.value.logoUrl = "/api" + res.data.url;
        if (res.success) message("Logo上传成功", { type: "success" });
        else message("上传失败: " + res.data.message, { type: "error" });
      })
      .catch(() => {
        message("Logo上传失败", { type: "error" });
      });
  }

  function handleTopImgChange(file: any) {
    const formData = new FormData();
    formData.append("file", file.raw);
    uploadHomeImage(formData)
      .then((res: any) => {
        siteForm.value.topImgUrl = "/api" + res.data.url;
        if (res.success) message("首页顶部图片上传成功", { type: "success" });
        else message("上传失败: " + res.data.message, { type: "error" });
      })
      .catch(() => {
        message("首页顶部图片上传失败", { type: "error" });
      });
  }

  async function saveSiteSettings() {
    const { success } = await saveSettings({
      settings: {
        school_name: siteForm.value.schoolName,
        web_title: siteForm.value.homeTitle,
        web_logo: siteForm.value.logoUrl,
        home_title_image: siteForm.value.topImgUrl,
        is_show_home_title_image: siteForm.value.showHomeTitle ? "1" : "0"
      }
    });
    if (success) {
      message("网站设置已保存", { type: "success" });
    } else {
      message("网站设置保存失败", { type: "error" });
    }
  }

  async function saveEntrySettings() {
    if (!entryForm.value.enableCanteen) {
      entryForm.value.enableAddDish = false;
    }
    const { success } = await saveSettings({
      settings: {
        is_show_canteen_entrance: entryForm.value.enableCanteen ? "1" : "0",
        is_show_dish_add_entrance: entryForm.value.enableAddDish ? "1" : "0"
      }
    });
    if (success) {
      message("入口开关设置已保存", { type: "success" });
    } else {
      message("入口开关设置保存失败", { type: "error" });
    }
  }

  async function saveAiSettings() {
    const { success } = await saveSettings({
      settings: {
        ai_api_link: aiForm.value.aiApiUrl,
        ai_api_key: aiForm.value.aiApiKey,
        ai_model_name: aiForm.value.aiModelName
      }
    });
    if (success) {
      message("AI调用设置已保存", { type: "success" });
    } else {
      message("AI调用设置保存失败", { type: "error" });
    }
  }

  async function saveRecordSettings() {
    const { success } = await saveSettings({
      settings: {
        icp_num: recordForm.value.icpNum,
        beian_mps: recordForm.value.publicSecurityNum
      }
    });
    if (success) {
      message("备案设置已保存", { type: "success" });
    } else {
      message("备案设置保存失败", { type: "error" });
    }
  }

  async function saveAccountSettings() {
    if (
      !accountForm.value.password ||
      accountForm.value.password.length < 8 ||
      accountForm.value.password.length > 22
    ) {
      message("密码不能为空且长度必须在8到22个字符之间", { type: "warning" });
      return;
    }
    const { success } = await setAdminPassword({
      type: accountForm.value.account,
      password: accountForm.value.password
    });
    if (success) {
      message("账号设置已保存", { type: "success" });
    } else {
      message("账号设置保存失败", { type: "error" });
    }
  }

  async function testApi() {
    isTesting.value = true;
    try {
      const response = await fetch(aiForm.value.aiApiUrl, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${aiForm.value.aiApiKey}`
        },
        body: JSON.stringify({
          model: aiForm.value.aiModelName,
          messages: [
            {
              role: "user",
              content:
                "请用{reply:string, version:string}的json格式回答本问题：你好"
            }
          ],
          response_format: { type: "json_object" }
        })
      });
      const data = await response.json();
      console.log(data);
      if (
        response.ok &&
        data &&
        Array.isArray(data.choices) &&
        typeof data.choices[0]?.message?.content === "string"
      ) {
        // 尝试解析返回内容为json
        try {
          const jsonContent = JSON.parse(data.choices[0].message.content);
          if (jsonContent.reply && jsonContent.version) {
            message("AI API测试成功", { type: "success" });
          } else {
            message("AI API测试失败，返回内容不符合预期", { type: "error" });
          }
        } catch {
          message("AI API测试失败，返回内容不是有效的JSON", { type: "error" });
        }
      } else {
        message("AI API测试失败: " + data.message, { type: "error" });
      }
    } catch (error) {
      message("AI API测试失败", { type: "error" });
    } finally {
      isTesting.value = false;
    }
  }

  return {
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
  };
}
