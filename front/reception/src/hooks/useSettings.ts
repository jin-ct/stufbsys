import { getSettings } from '@/api/settings';
import { reactive } from 'vue';

export function useSettings() {
  type Settings = {
    school_name: string;
    web_title: string;
    web_logo: string;
    home_title_image: string;
    is_show_home_title_image: boolean;
    is_show_canteen_entrance: boolean;
    is_show_dish_add_entrance: boolean;
    icp_num: string;
    beian_mps: string;
    [key: string]: string | boolean;
  };

  const settings = reactive<Settings>({
    school_name: '',
    web_title: '学生反馈系统',
    web_logo: '/favicon.ico',
    home_title_image: '/banner.png',
    is_show_home_title_image: true,
    is_show_canteen_entrance: true,
    is_show_dish_add_entrance: true,
    icp_num: '',
    beian_mps: '',
  });

  const loadSettingFormLocalStorage = () => {
    Object.keys(settings).forEach((key) => {
      const value = localStorage.getItem(key);
      if (value !== null && value !== '') {
        if (key.startsWith('is_')) {
          settings[key] = value === '1';
        } else {
          settings[key] = value;
        }
      }
    });
  };

  const updateWebIcon = () => {
    const link = document.querySelector("link[rel='icon']") as HTMLLinkElement;
    if (link) {
      link.href = settings.web_logo;
    } else {
      const newLink = document.createElement('link');
      newLink.rel = 'icon';
      newLink.href = settings.web_logo;
      document.head.appendChild(newLink);
    }
  };

  const applyBaseSettings = () => {
    document.title = settings.web_title;
    updateWebIcon();
  };

  const updateSettings = async () => {
    loadSettingFormLocalStorage();
    const response = await getSettings();
    const settingsData = response.data.data;
    if (response && settingsData && response.data.success) {
      Object.keys(settings).forEach((key) => {
        if (key in settingsData) {
          if (key.startsWith('is_')) {
            settings[key] = settingsData[key] === '1';
          } else {
            if (settingsData[key] !== '') settings[key] = settingsData[key];
          }
          localStorage.setItem(key, String(settingsData[key]));
        }
      });
    }
    applyBaseSettings();
  };

  updateSettings();

  return {
    settings,
    updateSettings,
    applyBaseSettings,
  };
}
