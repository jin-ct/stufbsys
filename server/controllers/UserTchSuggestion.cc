#include "UserTchSuggestion.h"

void UserTchSuggestion::getTeacherList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Teacher List";
	auto searchContent = "%" + req->getOptionalParameter<std::string>("search").value_or("") + "%";
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Teacher> mp(dbClientPtr);
	auto criteria = Criteria(Teacher::Cols::_name, CompareOperator::Like, searchContent) ||
					Criteria(Teacher::Cols::_subject, CompareOperator::Like, searchContent) ||
					Criteria(Teacher::Cols::_grade, CompareOperator::Like, searchContent) ||
					Criteria(Teacher::Cols::_class, CompareOperator::Like, searchContent);
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(Teacher::Cols::_name, SortOrder::ASC).paginate(page, pageSize).findBy(
		criteria,
		[callbackPtr, total](const std::vector<Teacher>& teachers) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& teacher : teachers) {
				Json::Value teacherJson;
				teacherJson["id"] = teacher.getValueOfTeacherId();
				teacherJson["subject"] = teacher.getValueOfSubject();
				teacherJson["class"] = teacher.getValueOfClass();
				teacherJson["grade"] = teacher.getValueOfGrade();
				teacherJson["name"] = teacher.getValueOfName();
				list.append(teacherJson);
			}
			data["teachers"] = list;
			if(total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Teacher List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserTchSuggestion::getSuggestionlist(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Teacher Suggestion List";
	auto teacherId = req->getOptionalParameter<int64_t>("teacherId").value_or(0);
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<TeacherSuggestion> mp(dbClientPtr);
	auto criteria = Criteria(TeacherSuggestion::Cols::_teacher_id, CompareOperator::EQ, teacherId) &&
					Criteria(TeacherSuggestion::Cols::_is_public, CompareOperator::EQ, true);
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(TeacherSuggestion::Cols::_date, SortOrder::DESC).paginate(page, pageSize).findBy(
		criteria,
		[callbackPtr, total](const std::vector<TeacherSuggestion>& suggestions) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& suggestion : suggestions) {
				Json::Value suggestionJson;
				suggestionJson["id"] = suggestion.getValueOfTchSugId();
				suggestionJson["title"] = suggestion.getValueOfTitle();
				suggestionJson["content"] = suggestion.getValueOfContent();
				suggestionJson["isAnonymous"] = suggestion.getValueOfIsAnonymous();
				suggestionJson["name"] = suggestion.getValueOfName();
				suggestionJson["class"] = suggestion.getValueOfClass();
				suggestionJson["vote"] = suggestion.getValueOfVoteCount();
				suggestionJson["date"] = suggestion.getDate()->toDbStringLocal();
				list.append(suggestionJson);
			}
			data["suggestions"] = list;
			if(total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Teacher Suggestion List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserTchSuggestion::addSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Add Teacher Suggestion";
	auto json = req->getJsonObject();
	if (!json || !json->isMember("teacherId") || !json->isMember("title") || !json->isMember("content")) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto teacherId = (*json)["teacherId"].asInt64();
	auto title = (*json)["title"].asString();
	auto content = (*json)["content"].asString();
	auto isAnonymous = (*json)["isAnonymous"].asBool();
	auto isPublic = (*json)["isPublic"].asBool();
	if (teacherId <= 0 || title.empty() || content.empty()) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<TeacherSuggestion> mp(dbClientPtr);
	TeacherSuggestion suggestion;
	suggestion.setTeacherId(teacherId);
	suggestion.setTitle(title);
	suggestion.setContent(content);
	suggestion.setIsPublic(isPublic);
	suggestion.setIsAnonymous(isAnonymous);
	if (!isAnonymous) {
		auto name = (*json)["name"].asString();
		auto className = (*json)["className"].asString();
		suggestion.setName(name);
		suggestion.setClass(className);
	}
	mp.insert(
		suggestion,
		[callbackPtr, teacherId](const TeacherSuggestion& s) {
			(*callbackPtr)(getJsonDataResponse(true));
			dbDataIncrementOne("teacher_data_statistics", "teacher_all_suggestion_num");
			dbDataIncrementOne("teacher_data_statistics", "teacher_suggestion_num", true, "teacher_id", teacherId);
			LOG_DEBUG << "Add Teacher Suggestion success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value res;
			res["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, res, drogon::k500InternalServerError));
		}
	);
}

void UserTchSuggestion::addVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Add Vote";
	auto teacherId = req->getOptionalParameter<int64_t>("teacherId").value_or(0);
	auto objectId = req->getOptionalParameter<int64_t>("id").value_or(0);
	auto userId = req->attributes()->get<int64_t>("userId");
	if (teacherId == 0 || objectId == 0 || userId == 0) {
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
		Criteria(VoteRecord::Cols::_type, CompareOperator::EQ, int16_t(2)),
		[callbackPtr, teacherId, objectId, userId](const std::vector<VoteRecord>& searchRes) {
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
			voteRecord.setType(2);
			voteRecord.setObjectId(objectId);
			voteRecord.setUserId(userId);
			mp.insert(
				voteRecord,
				[callbackPtr, teacherId, objectId](const VoteRecord& v) {
					(*callbackPtr)(getJsonDataResponse(true));
					dbDataIncrementOne("teacher_suggestion", "vote_count", false, "tch_sug_id", objectId);
					dbDataIncrementOne("teacher_data_statistics", "teacher_vote_num", true, "teacher_id", teacherId);
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