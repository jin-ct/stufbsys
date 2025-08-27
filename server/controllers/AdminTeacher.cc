#include "AdminTeacher.h"

namespace drogon {
	template<>
	inline Teacher fromRequest(const HttpRequest& req) {
		auto json = req.getJsonObject();
		auto t = Teacher(*json);
		return t;
	}
}

void AdminTeacher::getTeacherList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Teacher List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto searchContent = "%" + (*data)["search"]["content"].asString() + "%";
		auto criteria = Criteria(Teacher::Cols::_name, CompareOperator::Like, searchContent) ||
			Criteria(Teacher::Cols::_subject, CompareOperator::Like, searchContent) ||
			Criteria(Teacher::Cols::_grade, CompareOperator::Like, searchContent) ||
			Criteria(Teacher::Cols::_class, CompareOperator::Like, searchContent);

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Teacher> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);
		mp.orderBy(Teacher::Cols::_teacher_id, SortOrder::ASC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<Teacher>& teachers) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& teacher : teachers) {
					Json::Value teacherJson;
					teacherJson["id"] = teacher.getValueOfTeacherId();
					teacherJson["name"] = teacher.getValueOfName();
					teacherJson["subject"] = teacher.getValueOfSubject();
					teacherJson["grade"] = teacher.getValueOfGrade();
					teacherJson["class"] = teacher.getValueOfClass();
					teacherJson["email"] = teacher.getValueOfEmail();
					list.append(teacherJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Teacher List success";
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

void AdminTeacher::addTeacher(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Teacher&& teacher) const
{
	LOG_DEBUG << "Add Teacher";
	auto defaultPassword = BCrypt::generateHash(drogon::app().getCustomConfig()["defaultTeacherPassword"].asString());
	teacher.setPassword(defaultPassword);

	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Teacher> mp(dbClientPtr);
	mp.insert(
		teacher,
		[callbackPtr](const Teacher& t) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Add Teacher success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminTeacher::updateTeacher(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Teacher&& teacher) const
{
	LOG_DEBUG << "Update Teacher";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Teacher> mp(dbClientPtr);
	mp.update(
		teacher,
		[callbackPtr](const std::size_t count) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Update Teacher success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminTeacher::deleteTeacher(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Teacher";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Teacher> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(Teacher::Cols::_teacher_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Teacher success";
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

void AdminTeacher::importTeachers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Import Teachers";
	auto json = req->getJsonObject();
	auto defaultPassword = BCrypt::generateHash(drogon::app().getCustomConfig()["defaultTeacherPassword"].asString());
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	if (json && json->isMember("teachers")) {
		auto teachers = (*json)["teachers"];
		auto isCover = (*json)["isCover"].asBool();
		std::string sql = "INSERT INTO teacher (name, subject, grade, class, email, password) VALUES ";
		for (const auto& teacher : teachers) {
			sql += "('" + teacher["name"].asString() + "', '" +
				teacher["subject"].asString() + "', '" +
				teacher["grade"].asString() + "', '" +
				teacher["class"].asString() + "', '" +
				teacher["email"].asString() + "', '" +
				defaultPassword + "'),";
		}
		sql.pop_back();
		sql += ";";
		if (isCover) {
			auto transPtr = dbClientPtr->newTransaction();
			transPtr->execSqlAsync(
				"TRUNCATE TABLE teacher RESTART IDENTITY;",
				[transPtr, sql, callbackPtr](const drogon::orm::Result&) {
					transPtr->execSqlAsync(
						sql,
						[callbackPtr](const drogon::orm::Result&) {
							Json::Value data;
							data["message"] = "成功";
							(*callbackPtr)(getJsonDataResponse(true, data));
							LOG_DEBUG << "Import Teachers success";
						},
						[callbackPtr](const drogon::orm::DrogonDbException& e) {
							LOG_ERROR << "Database error: " << e.base().what();
							Json::Value data;
							data["message"] = "系统错误";
							(*callbackPtr)(getJsonDataResponse(false, data));
						}
					);
				},
				[callbackPtr](const drogon::orm::DrogonDbException& e) {
					LOG_ERROR << "Database error: " << e.base().what();
					Json::Value data;
					data["message"] = "系统错误";
					(*callbackPtr)(getJsonDataResponse(false, data));
				}
			);
		}
		else {
			dbClientPtr->execSqlAsync(
				sql,
				[callbackPtr](const drogon::orm::Result&) {
					Json::Value data;
					data["message"] = "成功";
					(*callbackPtr)(getJsonDataResponse(true, data));
					LOG_DEBUG << "Import Teachers success";
				},
				[callbackPtr](const drogon::orm::DrogonDbException& e) {
					LOG_ERROR << "Database error: " << e.base().what();
					Json::Value data;
					data["message"] = "系统错误";
					(*callbackPtr)(getJsonDataResponse(false, data));
				}
			);
		}
	}
	else {
		LOG_DEBUG << "Invalid request data";
		Json::Value data{};
		data["message"] = "数据错误";
		(*callbackPtr)(getJsonDataResponse(false, data));
	}
}

void AdminTeacher::resetPassword(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Reset Teacher Password";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json && json->isMember("id")) {
		auto id = (*json)["id"].asInt64();
		auto dbClientPtr = drogon::app().getDbClient();
		auto defaultPassword = drogon::app().getCustomConfig()["defaultTeacherPassword"].asString();
		Teacher teacher;
		teacher.setTeacherId(id);
		teacher.setPassword(BCrypt::generateHash(defaultPassword));

		Mapper<Teacher> mp(dbClientPtr);
		mp.update(
			teacher,
			[callbackPtr, defaultPassword](const std::size_t count) {
				Json::Value data{};
				data["message"] = "操作成功，默认密码：" + defaultPassword;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Reset Teacher Password success";
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
