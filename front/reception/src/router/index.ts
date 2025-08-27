import { createRouter, createWebHistory } from 'vue-router';
import NProgress from 'nprogress';
import 'nprogress/nprogress.css';
import HomeView from '../views/home/index.vue';
import { incrementVisitCount } from '@/api/settings';

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView,
    },
    {
      path: '/canteen',
      name: 'canteen',
      component: () => import('../views/canteen/index.vue'),
    },
    {
      path: '/canteen/evaluate-list/:id',
      name: 'canteen-evaluate-list',
      props: true,
      component: () => import('../views/canteen/evaluate-list.vue'),
    },
    {
      path: '/canteen/add-dish',
      name: 'canteen-add-dish',
      component: () => import('../views/canteen/add-dish.vue'),
    },
    {
      path: '/for-school',
      name: 'for-school',
      component: () => import('../views/for-school/index.vue'),
    },
    {
      path: '/for-school/short-list',
      name: 'for-school-short-list',
      component: () => import('../views/for-school/short-list.vue'),
    },
    {
      path: '/for-school/long-list',
      name: 'for-school-long-list',
      component: () => import('../views/for-school/long-list.vue'),
    },
    {
      path: '/for-teacher',
      name: 'for-teacher',
      component: () => import('../views/for-teacher/index.vue'),
    },
    {
      path: '/for-teacher/list/:id',
      name: 'for-teacher-list',
      props: true,
      component: () => import('../views/for-teacher/list.vue'),
    },
  ],
});

const incrementVisit = async () => {
  if (localStorage.getItem('userId')) {
    const lastincrementDate = new Date(localStorage.getItem('lastincrementDate') ?? '');
    const today = new Date();
    if (
      today.getFullYear() !== lastincrementDate.getFullYear() ||
      today.getMonth() !== lastincrementDate.getMonth() ||
      today.getDate() !== lastincrementDate.getDate()
    ) {
      await incrementVisitCount();
      localStorage.setItem('lastincrementDate', today.toDateString());
    }
  }
};

NProgress.configure({ showSpinner: false });

router.beforeEach((to, from, next) => {
  NProgress.start();
  next();
});

router.afterEach(() => {
  NProgress.done();
  incrementVisit();
});

export default router;
