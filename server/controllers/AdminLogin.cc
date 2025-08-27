#include "AdminLogin.h"

namespace drogon {
    template<>
    inline AdminLogin::User fromRequest(const HttpRequest& req) {
        auto json = req.getJsonObject();
        AdminLogin::User user;
        if (json)
        {
            user.username = (*json)["username"].asString();
            user.password = (*json)["password"].asString();
        }
        return user;
    }
}

void AdminLogin::login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, AdminLogin::User&& pNewUser) const
{
	LOG_DEBUG << "New administrator login";
	auto pNewUserPtr = std::make_shared<AdminLogin::User>(std::move(pNewUser));

    auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
    auto dbClientPtr = drogon::app().getDbClient();
    Mapper<AdminAccount> mp(dbClientPtr);
	mp.findBy(
		Criteria(AdminAccount::Cols::_username, CompareOperator::EQ, pNewUserPtr->username),
		[callbackPtr, pNewUserPtr](const std::vector<AdminAccount>& accounts) {
			if (accounts.empty()) {
				Json::Value data{};
				data["message"] = "用户名或密码错误";
				(*callbackPtr)(getJsonDataResponse(false, data));
				LOG_DEBUG << "Username or Password error";
			}
			else if (BCrypt::validatePassword(pNewUserPtr->password, *(accounts[0].getPassword()))) {
				Json::Value data{};
				std::string token = getToken(accounts[0]);
				data["accessToken"] = token;
				data["refreshToken"] = token;
				data["username"] = accounts[0].getValueOfUsername();
				data["nickname"] = accounts[0].getValueOfNickname();
				data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt() - 300);
				data["roles"] = Json::Value(Json::arrayValue);
				data["roles"].append("admin");
				data["permissions"] = Json::Value(Json::arrayValue);
				data["permissions"].append("*:*:*");
				data["avatar"] = "";
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

void AdminLogin::refreshTokenApi(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Refresh Token Api";
	auto json = req->getJsonObject();
	if (json) {
		auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
		auto jwt = jwtPtr->init();
		auto decoded = jwt.decode((*json)["refreshToken"].asString());
		if (decoded) {
			auto userId = decoded.value().get_payload_json()["userId"].to_str();
			auto role = decoded.value().get_payload_json()["role"].to_str();
			Json::Value data{};
			auto token = jwt.encode(std::stoi(userId), std::stoi(role));
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

void AdminLogin::getAsyncRoutes(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Aysnc Routes";
	auto token = req->getHeader("Authorization");
	const std::string prefix = "Bearer ";
	token = token.substr(prefix.size());
	if (!token.empty()) {
		auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
		auto jwt = jwtPtr->init();
		auto decoded = jwt.decode(token);
		if (decoded) {
			auto role = decoded.value().get_payload_json()["role"].to_str();
			Json::Value data(Json::arrayValue);
			switch (std::stoi(role))
			{
			case 0: // admin
				data.append(parseJsonString(canteenRouterJsonStr).value());
				data.append(parseJsonString(suggestionsRouterJsonStr).value());
				data.append(parseJsonString(noticesRouterJsonStr).value());
				data.append(parseJsonString(teachersRouterJsonStr).value());
				data.append(parseJsonString(platformRouterJsonStr).value());
				break;
			case 1: // baseadmin
				data.append(parseJsonString(canteenRouterJsonStr).value());
				data.append(parseJsonString(suggestionsRouterJsonStr).value());
				data.append(parseJsonString(noticesRouterJsonStr).value());
				data.append(parseJsonString(teachersRouterJsonStr).value());
				break;
			case 2: // suggestionadmin
				data.append(parseJsonString(suggestionsRouterJsonStr).value());
				break;
			case 3: // canteenadmin
				data.append(parseJsonString(canteenRouterJsonStr).value());
				break;
			}
			callback(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get AysncRoutes success";
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

void AdminLogin::getAdminResetVerificationInf(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Admin Reset Verification Info";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<PlatformSettings> mp(dbClientPtr);
	mp.findBy(
		Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_reset_verification_code") ||
		Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_verification_teachers"),
		[callbackPtr](const std::vector<PlatformSettings>& settings) {
			Json::Value data{};
			std::string code = "";
			size_t teachers_count = 0;
			for (const auto& setting : settings) {
				if (setting.getValueOfName() == "admin_reset_verification_code")
					code = setting.getValueOfValue();
				if (setting.getValueOfName() == "admin_verification_teachers")
					teachers_count = split(setting.getValueOfValue(), ',').size();
			}
			data["code"] = code;
			data["teachers_count"] = teachers_count;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Admin Reset Verification Info success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false));
		}
	);
}

void AdminLogin::adminReset(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Reset Admin Account Password";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto data = req->getJsonObject();
	if (data) {
		auto password_encrypted = BCrypt::generateHash((*data)["password"].asString());
		auto verification_code = generateRandomNumber(4);
		auto dbClientPtr = drogon::app().getDbClient();
		std::string sql = R"(
			UPDATE platform_settings
			SET value = CASE name
				WHEN 'admin_reset_verification_code' THEN $1
				WHEN 'admin_password_to_be_reset' THEN $2
				ELSE value
			END
			WHERE name IN ('admin_reset_verification_code', 'admin_password_to_be_reset')
		)";
		dbClientPtr->execSqlAsync(
			sql,
			[callbackPtr, verification_code, dbClientPtr](const drogon::orm::Result& result) {
				StartAdminResetTimeOut();
				Json::Value data{};
				data["verification_code"] = verification_code;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Admin reset password success";
			},
			[callbackPtr](const drogon::orm::DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false));
			},
			verification_code,
			password_encrypted
		);
	}
	else {
		LOG_DEBUG << "Invalid request data";
		(*callbackPtr)(getJsonDataResponse(false));
	}
}

std::string AdminLogin::getToken(const AdminAccount& user)
{
	auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
	auto jwt = jwtPtr->init();
	auto token = jwt.encode(*(user.getAdminId()), *(user.getType()));
	return token;
}

void AdminLogin::StartAdminResetTimeOut()
{
	drogon::app().getLoop()->runAfter(
		3600.0,
		[]() {
			LOG_DEBUG << "Admin Reset time out";
			std::string sql = R"(
				UPDATE platform_settings
				SET value = CASE name
					WHEN 'admin_reset_verification_code' THEN ''
					WHEN 'admin_password_to_be_reset' THEN ''
					WHEN 'admin_verification_teachers' THEN ''
					ELSE value
				END
				WHERE name IN ('admin_reset_verification_code', 'admin_password_to_be_reset', 'admin_verification_teachers')
			)";
			auto dbClientPtr = drogon::app().getDbClient();
			dbClientPtr->execSqlAsync(
				sql,
				[](const drogon::orm::Result&) {
					LOG_DEBUG << "Admin Reset closed";
				},
				[](const drogon::orm::DrogonDbException& e) {
					LOG_ERROR << "Database error: " << e.base().what();
				}
			);
		}
	);
}
