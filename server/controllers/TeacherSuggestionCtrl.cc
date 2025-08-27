#include "TeacherSuggestionCtrl.h"

void TeacherSuggestionCtrl::getSuggestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Teacher Suggestion List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto teacherId = req->attributes()->get<int64_t>("teacherId");
		auto searchTitle = "%" + (*data)["search"]["title"].asString() + "%";
		auto searchDate = (*data)["search"]["date"];
		auto criteria = Criteria(TeacherSuggestion::Cols::_title, CompareOperator::Like, searchTitle) &&
			Criteria(TeacherSuggestion::Cols::_teacher_id, CompareOperator::EQ, teacherId);
		if (!searchDate[0].asString().empty()) criteria = criteria &&
			Criteria(TeacherSuggestion::Cols::_date, CompareOperator::GE, searchDate[0].asString()) &&
			Criteria(TeacherSuggestion::Cols::_date, CompareOperator::LE, searchDate[1].asString());

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<TeacherSuggestion> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);
		mp.orderBy(TeacherSuggestion::Cols::_tch_sug_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<TeacherSuggestion>& suggestions) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& suggestion : suggestions) {
					Json::Value sugJson{};
					sugJson["id"] = suggestion.getValueOfTchSugId();
					sugJson["title"] = suggestion.getValueOfTitle();
					sugJson["votes"] = suggestion.getValueOfVoteCount();
					sugJson["isPublic"] = suggestion.getValueOfIsPublic();
					sugJson["isAnonymous"] = suggestion.getValueOfIsAnonymous();
					sugJson["date"] = suggestion.getDate()->toDbStringLocal();
					sugJson["content"] = suggestion.getValueOfContent();
					sugJson["name"] = suggestion.getValueOfName();
					sugJson["class"] = suggestion.getValueOfClass();
					list.append(sugJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Teacher Suggestion success";
			},
			[callbackPtr](const DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
			}
		);
	}
	else {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
	}
}

void TeacherSuggestionCtrl::deleteSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Teacher Suggestion";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<TeacherSuggestion> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(TeacherSuggestion::Cols::_tch_sug_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Teacher Suggestion success";
			},
			[callbackPtr](const DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false));
			}
		);
	}
	else
	{
		LOG_DEBUG << "Invalid request data";
		(*callbackPtr)(getJsonDataResponse(false));
	}
}
