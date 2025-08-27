#include "AdminWelcome.h"

void AdminWelcome::getDatas(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Admin Welcome Data";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<DataStatistics> mp(dbClientPtr);
	mp.orderBy(DataStatistics::Cols::_date, SortOrder::DESC).findAll(
		[callbackPtr](const std::vector<DataStatistics>& data) {
			Json::Value result(Json::arrayValue);
			for (const auto& item : data) {
				Json::Value jsonItem;
				jsonItem["date"] = item.getValueOfDate().toDbStringLocal();
				jsonItem["week"] = item.getValueOfWeek();
				jsonItem["evaluate_num"] = item.getValueOfEvaluateNum();
				jsonItem["suggestion_num"] = item.getValueOfSuggestionNum();
				jsonItem["visit"] = item.getValueOfVisit();
				jsonItem["dish_add"] = item.getValueOfDishAddNum();
				result.append(jsonItem);
			}
			(*callbackPtr)(getJsonDataResponse(true, result));
			LOG_DEBUG << "Get Admin Welcome Data success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}
