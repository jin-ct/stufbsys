import { getStatisticsData } from "@/api/welcome";
import { reactive } from "vue";

import GroupLine from "~icons/ri/group-line";
import Question from "~icons/ri/question-answer-line";
import CheckLine from "~icons/ri/chat-check-line";
import Smile from "~icons/ri/star-smile-line";

const chartData = reactive([
  {
    icon: GroupLine,
    bgColor: "#effaff",
    color: "#41b6ff",
    duration: 2200,
    name: "访问人数",
    value: 0,
    percent: "+0%",
    data: [] // 平滑折线图数据
  },
  {
    icon: Question,
    bgColor: "#fff5f4",
    color: "#e85f33",
    duration: 1600,
    name: "教师建议总数",
    value: 0,
    percent: "+0%",
    data: []
  },
  {
    icon: CheckLine,
    bgColor: "#eff8f4",
    color: "#26ce83",
    duration: 1500,
    name: "我收到的建议",
    value: 0,
    percent: "+0%",
    data: []
  },
  {
    icon: Smile,
    bgColor: "#f6f4fe",
    color: "#7846e5",
    duration: 100,
    name: "对建议表示赞同数",
    value: 0,
    percent: "+0%",
    data: []
  }
]);

/** 分析概览 */
const barChartData = reactive([
  {
    visitData: [],
    allSuggestionsData: [],
    suggestionsData: [],
    date: []
  },
  {
    visitData: [],
    allSuggestionsData: [],
    suggestionsData: [],
    date: []
  }
]);

/** 我的建议占比 */
const progressData = reactive([
  {
    week: "无数据",
    percentage: 0,
    duration: 110,
    color: "#41b6ff"
  },
  {
    week: "无数据",
    percentage: 0,
    duration: 105,
    color: "#41b6ff"
  },
  {
    week: "无数据",
    percentage: 0,
    duration: 100,
    color: "#41b6ff"
  },
  {
    week: "无数据",
    percentage: 0,
    duration: 95,
    color: "#41b6ff"
  },
  {
    week: "无数据",
    percentage: 0,
    duration: 90,
    color: "#26ce83"
  },
  {
    week: "无数据",
    percentage: 0,
    duration: 85,
    color: "#26ce83"
  },
  {
    week: "无数据",
    percentage: 0,
    duration: 80,
    color: "#26ce83"
  }
]);

async function getDataFromServer() {
  const { success, data } = await getStatisticsData({});
  if (success) {
    // 访问人数
    chartData[0].value = data[0]?.visit ?? 0;
    chartData[0].data = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.visit ?? 0);
    const percent = data[1].visit
      ? ((data[0].visit - data[1].visit) / data[1].visit) * 100
      : 0;
    chartData[0].percent = `${percent > 0 ? "+" : ""}${percent.toFixed(0)}%`;
    // 教师建议总数
    chartData[1].value = data[0]?.all_suggestion_num ?? 0;
    chartData[1].data = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.all_suggestion_num ?? 0);
    const percent2 = data[1].all_suggestion_num
      ? ((data[0].all_suggestion_num - data[1].all_suggestion_num) /
          data[1].all_suggestion_num) *
        100
      : 0;
    chartData[1].percent = `${percent2 > 0 ? "+" : ""}${percent2.toFixed(0)}%`;
    // 我收到的建议
    chartData[2].value = data[0]?.suggestion_num ?? 0;
    chartData[2].data = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.suggestion_num ?? 0);
    const percent3 = data[1].suggestion_num
      ? ((data[0].suggestion_num - data[1].suggestion_num) /
          data[1].suggestion_num) *
        100
      : 0;
    chartData[2].percent = `${percent3 > 0 ? "+" : ""}${percent3.toFixed(0)}%`;
    // 对建议表示赞同数
    chartData[3].value = data[0]?.vote_num ?? 0;
    chartData[3].data = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.vote_num ?? 0);
    const percent4 = data[1].vote_num
      ? ((data[0].vote_num - data[1].vote_num) / data[1].vote_num) * 100
      : 0;
    chartData[3].percent = `${percent4 > 0 ? "+" : ""}${percent4.toFixed(0)}%`;
    // 分析概览
    barChartData[1].visitData = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.visit ?? 0);
    barChartData[1].allSuggestionsData = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.all_suggestion_num ?? 0);
    barChartData[1].suggestionsData = Array(7)
      .fill(0)
      .map((_, i) => data[6 - i]?.suggestion_num ?? 0);
    barChartData[1].date = Array(7)
      .fill("暂无数据")
      .map((_, i) => data[6 - i]?.date ?? "暂无数据");
    barChartData[0].visitData = Array(7)
      .fill(0)
      .map((_, i) => data[13 - i]?.visit ?? 0);
    barChartData[0].allSuggestionsData = Array(7)
      .fill(0)
      .map((_, i) => data[13 - i]?.all_suggestion_num ?? 0);
    barChartData[0].suggestionsData = Array(7)
      .fill(0)
      .map((_, i) => data[13 - i]?.suggestion_num ?? 0);
    barChartData[0].date = Array(7)
      .fill("暂无数据")
      .map((_, i) => data[13 - i]?.date ?? "暂无数据");
    // 我的建议占比
    progressData.forEach((item, index) => {
      const suggestion_rate =
        data[index] && data[index].all_suggestion_num > 0
          ? Math.round(
              (data[index].suggestion_num / data[index].all_suggestion_num) *
                100
            )
          : 0;
      item.percentage = suggestion_rate;
      item.week = data[index].week;
      item.color = suggestion_rate < 50 ? "#26ce83" : "#41b6ff";
    });
  }
}

export { chartData, barChartData, progressData, getDataFromServer };
