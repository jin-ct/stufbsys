<template>
  <div class="painting-container">
    <canvas
      ref="canvasRef"
      @mousedown="startDrawing"
      @mousemove="draw"
      @mouseup="stopDrawing"
      @mouseleave="stopDrawing"
      @touchstart="handleTouchStart"
      @touchmove="handleTouchMove"
      @touchend="handleTouchEnd"
    ></canvas>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, watch, nextTick } from 'vue';

const props = withDefaults(
  defineProps<{
    promptText?: string;
    fontSize?: number;
    lineColor?: string;
    lineWidth?: number;
    clearInterval?: number;
  }>(),
  {
    promptText: '请绘制',
    fontSize: 18,
    lineColor: '#000000',
    lineWidth: 2,
    clearInterval: 500,
  },
);

const emit = defineEmits<{
  (e: 'drawingEnd'): void;
}>();

const promptText = ref<string>(props.promptText);
const canvasRef = ref<HTMLCanvasElement | null>(null);
const ctx = ref<CanvasRenderingContext2D | null>(null);
const isDrawing = ref(false);
const hasDrawn = ref(false);
const lastX = ref(0);
const lastY = ref(0);
const lastDrawEndTime = ref<number | null>(null);
const points = ref<{ x: number; y: number }[]>([]);
const drawingEndTimer = ref<number | null>(null);
// 记录所有笔迹
const strokes = ref<{ x: number; y: number }[][]>([]);

const symbolType = ref<'对' | '错'>('错');

const answer = defineModel<string>('answer', { default: '错' });
const base64 = defineModel<string>('base64', { default: '' });

const emitDrawingEnd = () => {
  if (!canvasRef.value) return;
  // 创建临时canvas
  const tempCanvas = document.createElement('canvas');
  tempCanvas.width = canvasRef.value.width;
  tempCanvas.height = canvasRef.value.height;
  const tempCtx = tempCanvas.getContext('2d');
  if (tempCtx) {
    tempCtx.lineCap = 'round';
    tempCtx.strokeStyle = props.lineColor;
    tempCtx.lineWidth = props.lineWidth;
    // 重绘所有笔迹
    for (const stroke of strokes.value) {
      if (stroke.length > 1) {
        tempCtx.beginPath();
        tempCtx.moveTo(stroke[0].x, stroke[0].y);
        for (let i = 1; i < stroke.length; i++) {
          tempCtx.lineTo(stroke[i].x, stroke[i].y);
        }
        tempCtx.stroke();
      }
    }
  }
  base64.value = tempCanvas.toDataURL('image/png');
  analyzeSymbol();
  promptText.value = props.promptText + ': ' + symbolType.value;
  clearCanvas();
  emit('drawingEnd');
};

// 初始化画布
const initCanvas = () => {
  if (!canvasRef.value) return;
  ctx.value = canvasRef.value.getContext('2d');
  if (!ctx.value) return;
  const container = canvasRef.value.parentElement;
  if (container) {
    canvasRef.value.width = container.clientWidth;
    canvasRef.value.height = container.clientHeight;
  }
  drawPrompt();
};

// 绘制提示文字
const drawPrompt = () => {
  if (!ctx.value || !canvasRef.value || hasDrawn.value) return;
  ctx.value.clearRect(0, 0, canvasRef.value.width, canvasRef.value.height);
  ctx.value.fillStyle = '#999999';
  ctx.value.font = `${props.fontSize}px Arial`;
  ctx.value.textAlign = 'center';
  ctx.value.textBaseline = 'middle';
  const centerX = canvasRef.value.width / 2;
  const centerY = canvasRef.value.height / 2;
  ctx.value.fillText(promptText.value, centerX, centerY);
};

// 清空画布
const clearCanvas = () => {
  if (!ctx.value || !canvasRef.value) return;
  ctx.value.clearRect(0, 0, canvasRef.value.width, canvasRef.value.height);
  hasDrawn.value = false;
  strokes.value = [];
  drawPrompt();
};

// 开始绘制
const startDrawing = (e: MouseEvent) => {
  if (!ctx.value || !canvasRef.value) return;

  isDrawing.value = true;
  const rect = canvasRef.value.getBoundingClientRect();
  lastX.value = e.clientX - rect.left;
  lastY.value = e.clientY - rect.top;
  points.value = [];
  points.value.push({ x: lastX.value, y: lastY.value });
  strokes.value.push(points.value.slice());
};

// 绘制过程
const draw = (e: MouseEvent) => {
  if (!isDrawing.value || !ctx.value || !canvasRef.value) return;
  const rect = canvasRef.value.getBoundingClientRect();
  const currentX = e.clientX - rect.left;
  const currentY = e.clientY - rect.top;
  ctx.value.beginPath();
  ctx.value.moveTo(lastX.value, lastY.value);
  ctx.value.lineTo(currentX, currentY);
  ctx.value.strokeStyle = props.lineColor;
  ctx.value.lineWidth = props.lineWidth;
  ctx.value.lineCap = 'round';
  ctx.value.stroke();
  lastX.value = currentX;
  lastY.value = currentY;
  hasDrawn.value = true;
  points.value.push({ x: currentX, y: currentY });
  // 同步到strokes
  strokes.value[strokes.value.length - 1] = points.value.slice();

  // 重置定时器
  if (drawingEndTimer.value) clearTimeout(drawingEndTimer.value);
  drawingEndTimer.value = window.setTimeout(() => {
    emitDrawingEnd();
  }, 500);
};

// 结束绘制
const stopDrawing = () => {
  if (!isDrawing.value || !canvasRef.value) return;
  isDrawing.value = false;
  hasDrawn.value = true;
  lastDrawEndTime.value = Date.now();

  // 停止后也重置定时器
  if (drawingEndTimer.value) clearTimeout(drawingEndTimer.value);
  drawingEndTimer.value = window.setTimeout(() => {
    emitDrawingEnd();
  }, 500);
};

// 触摸事件处理
const handleTouchStart = (e: TouchEvent) => {
  e.preventDefault();
  if (!canvasRef.value) return;
  const touch = e.touches[0];
  const mouseEvent = new MouseEvent('mousedown', {
    clientX: touch.clientX,
    clientY: touch.clientY,
  });
  canvasRef.value.dispatchEvent(mouseEvent);
};

const handleTouchMove = (e: TouchEvent) => {
  e.preventDefault();
  if (!canvasRef.value) return;
  const touch = e.touches[0];
  const mouseEvent = new MouseEvent('mousemove', {
    clientX: touch.clientX,
    clientY: touch.clientY,
  });
  canvasRef.value.dispatchEvent(mouseEvent);
};

const handleTouchEnd = () => {
  if (!canvasRef.value) return;
  canvasRef.value.dispatchEvent(new MouseEvent('mouseup'));
};

const analyzeSymbol = () => {
  if (strokes.value.length === 0) {
    symbolType.value = '错';
    answer.value = symbolType.value;
    return;
  }

  // 合并所有点
  const allPoints = strokes.value.flat();
  if (allPoints.length < 2) {
    symbolType.value = '错';
    answer.value = symbolType.value;
    return;
  }

  // 计算整体角度分布
  const getAngle = (p1: { x: number; y: number }, p2: { x: number; y: number }) => {
    return (Math.atan2(p2.y - p1.y, p2.x - p1.x) * 180) / Math.PI;
  };

  // 判断“勾”：
  // 1. 只有一笔
  // 2. 角度先向下（右下），后向上（右上）
  // 3. 轨迹呈现“V”型
  if (strokes.value.length === 1) {
    const pts = strokes.value[0];
    if (pts.length >= 4) {
      // 分段
      const mid = Math.floor(pts.length / 2);
      const angle1 = getAngle(pts[0], pts[mid]);
      const angle2 = getAngle(pts[mid], pts[pts.length - 1]);
      // “勾”一般先向右下（30~90度），再向右上（-30~-90度）
      if (
        angle1 > 20 &&
        angle1 < 120 &&
        angle2 < -20 &&
        angle2 > -120 &&
        Math.abs(angle1 - angle2) > 60
      ) {
        symbolType.value = '对';
        answer.value = symbolType.value;
        return;
      }
    }
  }

  symbolType.value = '错';
  answer.value = symbolType.value;
};

// 监听提示文字变化
watch(
  () => props.promptText,
  () => {
    if (!hasDrawn.value) {
      clearCanvas();
    }
  },
);

onMounted(() => {
  nextTick(() => {
    initCanvas();
    window.addEventListener('resize', initCanvas);
  });
});
</script>

<style scoped>
.painting-container {
  position: relative;
  width: 100%;
  height: 100%;
  border: 1px solid #ddd;
  border-radius: 4px;
  background-color: white;
  overflow: hidden;
}

canvas {
  display: block;
  width: 100%;
  height: 100%;
  touch-action: none; /* 防止触摸时页面滚动 */
}
</style>
