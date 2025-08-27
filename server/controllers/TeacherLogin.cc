#include "TeacherLogin.h"

namespace drogon {
	template<>
	inline Teacher fromRequest(const HttpRequest& req) {
		auto json = req.getJsonObject();
		Teacher user;
		if (json)
		{
			user.setName((*json)["username"].asString());
			user.setPassword((*json)["password"].asString());
		}
		return user;
	}
}

void TeacherLogin::login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Teacher&& pNewUser) const
{
	LOG_DEBUG << "New Teacher login";
	auto pNewUserPtr = std::make_shared<Teacher>(std::move(pNewUser));

	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Teacher> mp(dbClientPtr);
	mp.findBy(
		Criteria(Teacher::Cols::_name, CompareOperator::EQ, pNewUserPtr->getValueOfName()),
		[callbackPtr, pNewUserPtr](const std::vector<Teacher>& accounts) {
			if (accounts.empty()) {
				Json::Value data{};
				data["message"] = "用户名或密码错误";
				(*callbackPtr)(getJsonDataResponse(false, data));
				LOG_DEBUG << "Username or Password error";
			}
			else if (BCrypt::validatePassword(pNewUserPtr->getValueOfPassword(), accounts[0].getValueOfPassword())) {
				Json::Value data{};
				std::string token = getToken(accounts[0]);
				data["accessToken"] = token;
				data["refreshToken"] = token;
				data["username"] = accounts[0].getValueOfName();
				data["nickname"] = accounts[0].getValueOfName();
				data["teacherId"] = *(accounts[0].getTeacherId());
				data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt() - 300);
				data["roles"] = Json::Value(Json::arrayValue);
				data["permissions"] = Json::Value(Json::arrayValue);
				data["permissions"].append("*:*:*");
				data["avatar"] = "";
				data["firstLogin"] = drogon::app().getCustomConfig()["defaultTeacherPassword"].asString() == pNewUserPtr->getValueOfPassword();
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Login success";
			}
			else {
				Json::Value data{};
				data["message"] = "用户名或密码错误";
				(*callbackPtr)(getJsonDataResponse(false, data));
				LOG_DEBUG << "Username or Password error";
			}
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void TeacherLogin::refreshTokenApi(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Refresh Teacher Token Api";
	auto json = req->getJsonObject();
	if (json) {
		auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
		auto jwt = jwtPtr->init();
		auto decoded = jwt.decode((*json)["refreshToken"].asString());
		if (decoded) {
			auto userId = decoded.value().get_payload_json()["teacher_id"].to_str();
			Teacher user;
			user.setTeacherId(std::stoi(userId));
			std::string token = getToken(user);
			Json::Value data{};
			data["accessToken"] = token;
			data["refreshToken"] = token;
			data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt() - 300);
			callback(getJsonDataResponse(true, data));
			LOG_DEBUG << "Refresh success";
		}
		else {
			callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
			LOG_DEBUG << "Refresh error";
		}
	}
}

void TeacherLogin::getAsyncRoutes(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Teacher Aysnc Routes";
	auto token = req->getHeader("Authorization");
	const std::string prefix = "Bearer ";
	token = token.substr(prefix.size());

	if (!token.empty()) {
		auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
		auto jwt = jwtPtr->init();
		auto decoded = jwt.decode(token);
		if (decoded) {
			Json::Value data(Json::arrayValue);
			data.append(parseJsonString(suggestionsRouterJsonStr).value());
			data.append(parseJsonString(platformRouterJsonStr).value());
			callback(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Teacher AsyncRoutes success";
		}
		else {
			callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
			LOG_DEBUG << "Token decode error";
		}
	}
	else {
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		LOG_DEBUG << "Token is empty";
	}
}

std::string TeacherLogin::getToken(const Teacher& user)
{
	auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
	auto jwt = jwtPtr->init();
	auto token = jwt.encode("teacher_id", *(user.getTeacherId()));
	return token;
}
