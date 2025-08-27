#include "EverydayTasks.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

void doList()
{
    updateStatisticalData();
    updateTeacherStatisticalData();
    updateTeacherBottomSaying();
}

void startEverydayTasks() {
    scheduleNextTask();
}

void scheduleNextTask() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &now_c);
#else
    localtime_r(&now_c, &tm);
#endif
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    auto next_midnight = std::chrono::system_clock::from_time_t(std::mktime(&tm)) + std::chrono::hours(24);
    auto seconds_until_midnight = std::chrono::duration_cast<std::chrono::seconds>(next_midnight - now).count();
    seconds_until_midnight += 10;  // 加10秒防止获取当前日期错误

    drogon::app().getLoop()->runAfter(
        seconds_until_midnight,
        []() {
            doList();
            scheduleNextTask();  // 递归调度下一次
        }
    );
}

std::string getWeekDay()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    switch (ltm->tm_wday) {
        case 0: return "周日";
        case 1: return "周一";
        case 2: return "周二";
        case 3: return "周三";
        case 4: return "周四";
        case 5: return "周五";
        case 6: return "周六";
        default: return "";
    }
}

void updateStatisticalData()
{
	LOG_DEBUG << "Updating Statistical Data";
	auto dbClientPtr = drogon::app().getDbClient();
    Mapper<DataStatistics> mp(dbClientPtr);
    auto dataCount = mp.count();
    if (dataCount >= 14) {
        dbClientPtr->newTransactionAsync(
            [dbClientPtr](const std::shared_ptr<drogon::orm::Transaction>& transPtr) {
                transPtr->execSqlAsync(
                    "DELETE FROM data_statistics WHERE date = (SELECT MIN(date) FROM data_statistics)",
                    [transPtr](const drogon::orm::Result& r) {
                        DataStatistics newData;
                        newData.setWeek(getWeekDay());

                        Mapper<DataStatistics> mp(transPtr);
                        mp.insert(
                            newData,
                            [](const DataStatistics& obj) {
                                LOG_DEBUG << "Insert new data success";
                            },
                            [](const drogon::orm::DrogonDbException& e) {
                                LOG_ERROR << "Insert failed: " << e.base().what();
                            }
                        );
                    },
                    [transPtr](const drogon::orm::DrogonDbException& e) {
                        LOG_ERROR << "Delete failed: " << e.base().what();
                    }
                );
            }
        );
    }
    else
    {
        DataStatistics newData;
        newData.setWeek(getWeekDay());
        mp.insert(
            newData,
            [](const DataStatistics& obj) {
                LOG_DEBUG << "Insert new data success";
            },
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Insert failed: " << e.base().what();
            }
        );
    }
}

void updateTeacherStatisticalData()
{
    LOG_DEBUG << "Updating Teacher Statistical Data";
    auto dbClientPtr = drogon::app().getDbClient();
    Mapper<TeacherDataStatistics> mp_tds(dbClientPtr);
    auto dataCount = mp_tds.count();
    Mapper<Teacher> mp_t(dbClientPtr);
    mp_t.findAll(
        [dataCount](const std::vector<Teacher>& teachers) {
            std::string sql = "INSERT INTO teacher_data_statistics (week, teacher_id) VALUES ";
            size_t teacherCount = teachers.size();
            if (teacherCount == 0) return;

			auto weekDay = getWeekDay();
            for (const auto& teacher : teachers) {
                sql += "('" + weekDay + "', " +
                    std::to_string(teacher.getValueOfTeacherId()) + "),";
            }
            sql.pop_back();

            auto dbClientPtr = drogon::app().getDbClient();
            if (dataCount >= 14*teacherCount) {
                auto transPtr = dbClientPtr->newTransaction();
                transPtr->execSqlAsync(
                    "DELETE FROM data_statistics WHERE date = (SELECT MIN(date) FROM data_statistics)",
                    [transPtr, sql](const drogon::orm::Result& r) {
                        transPtr->execSqlAsync(
                            sql,
                            [](const drogon::orm::Result& r) {
                                LOG_DEBUG << "Insert new data success";
                            },
                            [](const drogon::orm::DrogonDbException& e) {
                                LOG_ERROR << "Insert failed: " << e.base().what();
                            }
                        );
                    },
                    [](const drogon::orm::DrogonDbException& e) {
                        LOG_ERROR << "Delete failed: " << e.base().what();
                    }
                );
            }
            else
            {
                dbClientPtr->execSqlAsync(
                    sql,
                    [](const drogon::orm::Result& r) {
                        LOG_DEBUG << "Insert new data success";
                    },
                    [](const drogon::orm::DrogonDbException& e) {
                        LOG_ERROR << "Insert failed: " << e.base().what();
                    }
                );
            }
        },
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Get Teacher failed: " << e.base().what();
        }
    );
}

void updateTeacherBottomSaying()
{
    LOG_DEBUG << "Update Teacher Bottom Saying";
    Json::Value messages(Json::arrayValue);
    Json::Value message_sys;
    Json::Value message_user;
    message_sys["role"] = "system";
    message_sys["content"] = "请使用{\"saying\":string}的json格式回答，不使用缓存，请完全重新查找，你应当查找多个并随机挑选其中一个";
    message_user["role"] = "user";
    message_user["content"] = "请查找一句名言，以让老师缓解压力，安抚心灵，要求10到30字左右，以\"师\"为主题，最好有夸奖的意思";
    messages.append(message_sys);
    messages.append(message_user);
    requestAiApi(
        messages,
        [](const Json::Value res) {
            if (res["success"].asBool() && res["data"]["choices"].size() > 0) {
                auto content = parseJsonString(res["data"]["choices"][0]["message"]["content"].asString());
                if (!content.has_value() || !content.value().isMember("saying")) {
                    LOG_ERROR << "Ai Out Data error，retrying...";
                    drogon::app().getLoop()->runAfter(10.0, updateTeacherBottomSaying);
                    return;
                }
                auto saying = content.value()["saying"].asString();
                if (saying.find_last_of("。")) {
                    saying.pop_back();
                    saying.pop_back();
                    saying.pop_back();
                }
                auto dbClientPtr = drogon::app().getDbClient();
                Mapper<PlatformSettings> mp(dbClientPtr);
                std::vector<std::string> col;
                col.push_back("value");
                mp.updateBy(
                    col,
                    [](const std::size_t count) {
                        if (count > 0) LOG_DEBUG << "Update Teacher Bottom Saying success";
                    },
                    [](const DrogonDbException& e) {
                        LOG_ERROR << "Database error: " << e.base().what();
                    },
                    Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "teacher_admin_bottom_saying"),
                    saying
                );
            }
            else {
                LOG_ERROR << "Update error";
            }
        }
    );
}