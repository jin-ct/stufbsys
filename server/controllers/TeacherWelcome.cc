#include "TeacherWelcome.h"

void TeacherWelcome::getDatas(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Teacher Welcome Data";
	auto teacherId = req->attributes()->get<int64_t>("teacherId");
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<TeacherDataStatistics> mp(dbClientPtr);
	mp.orderBy(TeacherDataStatistics::Cols::_date, SortOrder::DESC).findBy(
		Criteria(TeacherDataStatistics::Cols::_teacher_id, CompareOperator::EQ, teacherId),
		[callbackPtr](const std::vector<TeacherDataStatistics>& data) {
			Json::Value result(Json::arrayValue);
			for (const auto& item : data) {
				Json::Value jsonItem;
				jsonItem["date"] = item.getValueOfDate().toDbStringLocal();
				jsonItem["week"] = item.getValueOfWeek();
				jsonItem["all_suggestion_num"] = item.getValueOfTeacherAllSuggestionNum();
				jsonItem["suggestion_num"] = item.getValueOfTeacherSuggestionNum();
				jsonItem["visit"] = item.getValueOfVisit();
				jsonItem["vote_num"] = item.getValueOfTeacherVoteNum();
				result.append(jsonItem);
			}
			(*callbackPtr)(getJsonDataResponse(true, result));
			LOG_DEBUG << "Get Teacher Welcome Data success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}
