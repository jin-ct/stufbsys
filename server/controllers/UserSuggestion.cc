#include "UserSuggestion.h"

void UserSuggestion::getShortSuggestionlist(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get School Short Suggestion List";
	auto searchContent = "%" + req->getOptionalParameter<std::string>("search").value_or("") + "%";
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<SchoolSuggestionShort> mp(dbClientPtr);
	auto criteria = Criteria(SchoolSuggestionShort::Cols::_is_public, CompareOperator::EQ, true) && (
					Criteria(SchoolSuggestionShort::Cols::_title, CompareOperator::Like, searchContent) ||
					Criteria("to_char(\"date\", 'YYYY-MM-DD') like $?"_sql, searchContent));
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(SchoolSuggestionShort::Cols::_date, SortOrder::DESC).paginate(page, pageSize).findBy(
		criteria,
		[callbackPtr, total](const std::vector<SchoolSuggestionShort>& suggestions) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& suggestion : suggestions) {
				Json::Value suggestionJson;
				suggestionJson["id"] = suggestion.getValueOfSchSugId();
				suggestionJson["title"] = suggestion.getValueOfTitle();
				suggestionJson["content"] = suggestion.getValueOfContent();
				suggestionJson["isAnonymous"] = suggestion.getValueOfIsAnonymous();
				suggestionJson["name"] = suggestion.getValueOfName();
				suggestionJson["class"] = suggestion.getValueOfClass();
				suggestionJson["grade"] = suggestion.getValueOfGrade();
				suggestionJson["vote"] = suggestion.getValueOfVoteCount();
				suggestionJson["type"] = suggestion.getValueOfType();
				suggestionJson["score"] = suggestion.getValueOfAiScore();
				suggestionJson["date"] = suggestion.getDate()->toDbStringLocal();
				list.append(suggestionJson);
			}
			data["suggestions"] = list;
			if(total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get School Short List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserSuggestion::addShortSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Add School Short Suggestion";
	auto json = req->getJsonObject();
	if (!json || !json->isMember("title") || !json->isMember("content")) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto title = (*json)["title"].asString();
	auto content = (*json)["content"].asString();
	auto isAnonymous = (*json)["isAnonymous"].asBool();
	auto isPublic = (*json)["isPublic"].asBool();
	auto type = (*json)["typeID"].asInt();
	if (title.empty() || content.empty()) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<SchoolSuggestionShort> mp(dbClientPtr);
	SchoolSuggestionShort suggestion;
	suggestion.setTitle(title);
	suggestion.setContent(content);
	suggestion.setType(type);
	suggestion.setIsPublic(isPublic);
	suggestion.setIsAnonymous(isAnonymous);
	if (!isAnonymous) {
		auto name = (*json)["name"].asString();
		auto className = (*json)["className"].asString();
		auto grade = (*json)["grade"].asString();
		suggestion.setName(name);
		suggestion.setClass(className);
		suggestion.setGrade(grade);
	}
	mp.insert(
		suggestion,
		[callbackPtr](const SchoolSuggestionShort& s) {
			(*callbackPtr)(getJsonDataResponse(true));
			dbDataIncrementOne("data_statistics", "suggestion_num");
			updateAiScore(s.getValueOfTitle(), s.getValueOfContent(), 3, s.getValueOfSchSugId());
			LOG_DEBUG << "Add School Short Suggestion success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value res;
			res["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, res, drogon::k500InternalServerError));
		}
	);
}

void UserSuggestion::getLongSuggestionlist(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get School Long Suggestion List";
	auto searchContent = "%" + req->getOptionalParameter<std::string>("search").value_or("") + "%";
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<SchoolSuggestionLong> mp(dbClientPtr);
	auto criteria = Criteria(SchoolSuggestionLong::Cols::_is_public, CompareOperator::EQ, true) && (
					Criteria(SchoolSuggestionLong::Cols::_title, CompareOperator::Like, searchContent) ||
					Criteria("to_char(\"date\", 'YYYY-MM-DD') like $?"_sql, searchContent));
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(SchoolSuggestionLong::Cols::_date, SortOrder::DESC).paginate(page, pageSize).findBy(
		criteria,
		[callbackPtr, total](const std::vector<SchoolSuggestionLong>& suggestions) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& suggestion : suggestions) {
				Json::Value suggestionJson;
				suggestionJson["id"] = suggestion.getValueOfSchSugId();
				suggestionJson["title"] = suggestion.getValueOfTitle();
				suggestionJson["content"] = suggestion.getValueOfContent();
				suggestionJson["isAnonymous"] = suggestion.getValueOfIsAnonymous();
				suggestionJson["name"] = suggestion.getValueOfName();
				suggestionJson["class"] = suggestion.getValueOfClass();
				suggestionJson["grade"] = suggestion.getValueOfGrade();
				suggestionJson["vote"] = suggestion.getValueOfVoteCount();
				suggestionJson["type"] = suggestion.getValueOfType();
				suggestionJson["score"] = suggestion.getValueOfAiScore();
				suggestionJson["date"] = suggestion.getDate()->toDbStringLocal();
				list.append(suggestionJson);
			}
			data["suggestions"] = list;
			if(total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get School Long List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserSuggestion::addLongSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Add School Long Suggestion";
	auto json = req->getJsonObject();
	if (!json || !json->isMember("title") || !json->isMember("content")) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto title = (*json)["title"].asString();
	auto content = (*json)["content"].asString();
	auto isAnonymous = (*json)["isAnonymous"].asBool();
	auto isPublic = (*json)["isPublic"].asBool();
	auto type = (*json)["typeID"].asInt();
	if (title.empty() || content.empty()) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<SchoolSuggestionLong> mp(dbClientPtr);
	SchoolSuggestionLong suggestion;
	suggestion.setTitle(title);
	suggestion.setContent(content);
	suggestion.setIsPublic(isPublic);
	suggestion.setType(type);
	suggestion.setIsAnonymous(isAnonymous);
	if (!isAnonymous) {
		auto name = (*json)["name"].asString();
		auto className = (*json)["className"].asString();
		auto grade = (*json)["grade"].asString();
		suggestion.setName(name);
		suggestion.setClass(className);
		suggestion.setGrade(grade);
	}
	mp.insert(
		suggestion,
		[callbackPtr](const SchoolSuggestionLong& s) {
			(*callbackPtr)(getJsonDataResponse(true));
			dbDataIncrementOne("data_statistics", "suggestion_num");
			updateAiScore(s.getValueOfTitle(), s.getValueOfContent(), 4, s.getValueOfSchSugId());
			LOG_DEBUG << "Add School Long Suggestion success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value res;
			res["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, res, drogon::k500InternalServerError));
		}
	);
}

void UserSuggestion::addVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Add Vote";
	auto type = req->getOptionalParameter<int16_t>("type").value_or(-1);
	auto objectId = req->getOptionalParameter<int64_t>("id").value_or(0);
	auto userId = req->attributes()->get<int64_t>("userId");
	if (type == -1 || objectId == 0 || userId == 0) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<VoteRecord> mp(dbClientPtr);
	mp.findBy(
		Criteria(VoteRecord::Cols::_user_id, CompareOperator::EQ, userId) &&
		Criteria(VoteRecord::Cols::_object_id, CompareOperator::EQ, objectId) &&
		Criteria(VoteRecord::Cols::_type, CompareOperator::EQ, type),
		[callbackPtr, type, objectId, userId](const std::vector<VoteRecord>& searchRes) {
			if (searchRes.size() > 0) {
				LOG_DEBUG << "Repeated Vote by the user";
				Json::Value res;
				res["message"] = "你已赞同过";
				(*callbackPtr)(getJsonDataResponse(false, res, drogon::k400BadRequest));
				return;
			}
			auto dbClientPtr = drogon::app().getDbClient();
			Mapper<VoteRecord> mp(dbClientPtr);
			VoteRecord voteRecord;
			voteRecord.setType(type);
			voteRecord.setObjectId(objectId);
			voteRecord.setUserId(userId);
			mp.insert(
				voteRecord,
				[callbackPtr, type, objectId](const VoteRecord& v) {
					(*callbackPtr)(getJsonDataResponse(true));
					switch (type) {
					case 3:
						dbDataIncrementOne("school_suggestion_short", "vote_count", false, "sch_sug_id", objectId);
						break;
					case 4:
						dbDataIncrementOne("school_suggestion_long", "vote_count", false, "sch_sug_id", objectId);
						break;
					}
					LOG_DEBUG << "Vote Add success";
				},
				[callbackPtr](const DrogonDbException& e) {
					LOG_ERROR << "Database error: " << e.base().what();
					Json::Value res;
					res["message"] = "系统错误";
					(*callbackPtr)(getJsonDataResponse(false, res, drogon::k500InternalServerError));
				}
			);
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value res;
			res["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, res, drogon::k500InternalServerError));
		}
	);
}

void UserSuggestion::updateAiScore(const std::string& title, const std::string& contentText, const short& typeId, const int64_t& sug_id, const int& tryCount)
{
	Json::Value messages(Json::arrayValue);
	Json::Value message_sys;
	Json::Value message_user;
	message_sys["role"] = "system";
	message_sys["content"] = R"(你需要对学生为学校提供的建议进行打分(1至5分，两位小数)，内容可能为富文本编辑器输出的html，请自行解析;
								你可以根据以下几方面进行综合评估：
								  1、内容描述是否清晰；
								  2、是否有合理的观点；
								  3、建议是否实际可行；
								  4、对学校发展的影响；
								请使用{"score":number}的json格式回答
								)";
	message_user["role"] = "user";
	message_user["content"] = "标题：" + title + " | 内容：" + contentText;
	messages.append(message_sys);
	messages.append(message_user);
	requestAiApi(
		messages,
		[title, contentText, typeId, sug_id, tryCount](const Json::Value res) {
			if (res["success"].asBool() && res["data"]["choices"].size() > 0) {
				auto content = parseJsonString(res["data"]["choices"][0]["message"]["content"].asString());
				if (!content.has_value() || !content.value().isMember("score")) {
					if (tryCount < 3) {
						LOG_ERROR << "Ai Out Data error，retrying(" << tryCount << ")...";
						drogon::app().getLoop()->runAfter(10.0, [title, contentText, typeId, sug_id, tryCount]() {
							UserSuggestion::updateAiScore(title, contentText, typeId, sug_id, tryCount + 1);
						});
					}
					return;
				}
				auto score = content.value()["score"].asDouble();

				auto dbClientPtr = drogon::app().getDbClient();
				std::string table = (typeId == 3) ? "school_suggestion_short" : "school_suggestion_long";
				std::string sql = "UPDATE \"" + table + "\" SET ai_score = $1 WHERE sch_sug_id = $2";
				dbClientPtr->execSqlAsync(
					sql,
					[typeId](const drogon::orm::Result& result) {
						if (result.affectedRows() > 0) LOG_DEBUG << "Update Ai Score success, type=" << typeId;
					},
					[typeId](const DrogonDbException& e) {
						LOG_ERROR << "Database error:  type=" << typeId << ", what=" << e.base().what();
					},
					score,
					sug_id
				);
			}
			else {
				LOG_ERROR << "Update Ai Score error, type=" << typeId;
			}
		}
	);
}