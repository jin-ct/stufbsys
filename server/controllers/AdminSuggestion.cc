#include "AdminSuggestion.h"

void AdminSuggestion::getShortSuggestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Short Suggestion List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto sortByScore = (*data)["sortByScore"];
		auto searchTitle = "%" + (*data)["search"]["title"].asString() + "%";
		auto searchType = (*data)["search"]["type"].asString();
		auto searchDate = (*data)["search"]["date"];
		auto criteria = Criteria(SchoolSuggestionShort::Cols::_title, CompareOperator::Like, searchTitle);
		if (searchType != "-1") criteria = criteria && Criteria(SchoolSuggestionShort::Cols::_type, CompareOperator::EQ, searchType);
		if (!searchDate[0].asString().empty()) criteria = criteria &&
			Criteria(SchoolSuggestionShort::Cols::_date, CompareOperator::GE, searchDate[0].asString()) &&
			Criteria(SchoolSuggestionShort::Cols::_date, CompareOperator::LE, searchDate[1].asString());

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<SchoolSuggestionShort> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);

		if (!sortByScore.empty()) {
			auto sortByScoreStr = sortByScore.asString();
			if (!sortByScoreStr.empty()) {
				if (sortByScoreStr == "asc") mp = mp.orderBy(SchoolSuggestionShort::Cols::_ai_score, SortOrder::ASC);
				else mp = mp.orderBy(SchoolSuggestionShort::Cols::_ai_score, SortOrder::DESC);
			}
		}
		mp.orderBy(SchoolSuggestionShort::Cols::_sch_sug_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<SchoolSuggestionShort>& suggestions) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& suggestion : suggestions) {
					Json::Value sugJson;
					sugJson["id"] = suggestion.getValueOfSchSugId();
					sugJson["title"] = suggestion.getValueOfTitle();
					sugJson["score"] = suggestion.getValueOfAiScore();
					sugJson["votes"] = suggestion.getValueOfVoteCount();
					sugJson["isPublic"] = suggestion.getValueOfIsPublic();
					sugJson["isAnonymous"] = suggestion.getValueOfIsAnonymous();
					sugJson["type"] = suggestion.getValueOfType();
					sugJson["date"] = suggestion.getDate()->toDbStringLocal();
					sugJson["content"] = suggestion.getValueOfContent();
					sugJson["name"] = suggestion.getValueOfName();
					sugJson["class"] = suggestion.getValueOfClass();
					sugJson["grade"] = suggestion.getValueOfGrade();
					list.append(sugJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Short Suggestion success";
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

void AdminSuggestion::deleteShortSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Short Suggestion";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<SchoolSuggestionShort> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(SchoolSuggestionShort::Cols::_sch_sug_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Short Suggestion success";
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

void AdminSuggestion::getLongSuggestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Long Suggestion List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto sortByScore = (*data)["sortByScore"];
		auto searchTitle = "%" + (*data)["search"]["title"].asString() + "%";
		auto searchType = (*data)["search"]["type"].asString();
		auto searchDate = (*data)["search"]["date"];
		auto criteria = Criteria(SchoolSuggestionLong::Cols::_title, CompareOperator::Like, searchTitle);
		if (searchType != "-1") criteria = criteria && Criteria(SchoolSuggestionLong::Cols::_type, CompareOperator::EQ, searchType);
		if (!searchDate[0].asString().empty()) criteria = criteria &&
			Criteria(SchoolSuggestionLong::Cols::_date, CompareOperator::GE, searchDate[0].asString()) &&
			Criteria(SchoolSuggestionLong::Cols::_date, CompareOperator::LE, searchDate[1].asString());

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<SchoolSuggestionLong> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);

		if (!sortByScore.empty()) {
			auto sortByScoreStr = sortByScore.asString();
			if (!sortByScoreStr.empty()) {
				if (sortByScoreStr == "asc") mp = mp.orderBy(SchoolSuggestionLong::Cols::_ai_score, SortOrder::ASC);
				else mp = mp.orderBy(SchoolSuggestionLong::Cols::_ai_score, SortOrder::DESC);
			}
		}
		mp.orderBy(SchoolSuggestionLong::Cols::_sch_sug_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<SchoolSuggestionLong>& suggestions) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& suggestion : suggestions) {
					Json::Value sugJson;
					sugJson["id"] = *(suggestion.getSchSugId());
					sugJson["title"] = *(suggestion.getTitle());
					sugJson["score"] = *(suggestion.getAiScore());
					sugJson["votes"] = *(suggestion.getVoteCount());
					sugJson["isPublic"] = *(suggestion.getIsPublic());
					sugJson["isAnonymous"] = *(suggestion.getIsAnonymous());
					sugJson["type"] = *(suggestion.getType());
					sugJson["date"] = suggestion.getDate()->toDbStringLocal();
					sugJson["content"] = *(suggestion.getContent());
					sugJson["name"] = suggestion.getName() == nullptr ? "" : *(suggestion.getName());
					sugJson["class"] = suggestion.getClass() == nullptr ? "" : *(suggestion.getClass());
					sugJson["grade"] = suggestion.getGrade() == nullptr ? "" : *(suggestion.getGrade());
					list.append(sugJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Long Suggestion success";
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

void AdminSuggestion::deleteLongSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Long Suggestion";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<SchoolSuggestionLong> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(SchoolSuggestionLong::Cols::_sch_sug_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Long Suggestion success";
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
