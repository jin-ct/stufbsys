#include "TeacherSettings.h"

void TeacherSettings::setSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Set Teacher Settings";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto data = req->getJsonObject();
	if (data) {
		auto teacherId = req->attributes()->get<int64_t>("teacherId");
		auto email = (*data)["email"].asString();
		auto password = (*data)["password"].asString();
		Teacher account;
		account.setTeacherId(teacherId);
		account.setEmail(email);
		if(!password.empty()) account.setPassword(BCrypt::generateHash(password));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Teacher> mp(dbClientPtr);
		mp.update(
			account,
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Set Teacher Settings success";
			},
			[callbackPtr](const DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false));
			}
		);
	}
	else {
		LOG_DEBUG << "Invalid request data";
		(*callbackPtr)(getJsonDataResponse(false));
	}
}

void TeacherSettings::getSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Settings";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json) {
		auto teacherId = std::to_string(req->attributes()->get<int64_t>("teacherId"));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<PlatformSettings> mp(dbClientPtr);
		mp.findBy(
			Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_reset_verification_code") ||
			Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_verification_teachers"),
			[callbackPtr, teacherId](const std::vector<PlatformSettings>& settings) {
				std::string code = "";
				std::string teachersStr = "";
				for (const auto& setting : settings) {
					if (setting.getValueOfName() == "admin_reset_verification_code")
						code = setting.getValueOfValue();
					if (setting.getValueOfName() == "admin_verification_teachers")
						teachersStr = setting.getValueOfValue();
				}
				auto teachers = split(teachersStr, ',');
				auto isTeacherVerifyed = std::find(teachers.begin(), teachers.end(), teacherId) != teachers.end();
				auto isReset = code != "" && !isTeacherVerifyed;
				Mapper<Teacher> mp(drogon::app().getDbClient());
				mp.findBy(
					Criteria(Teacher::Cols::_teacher_id, CompareOperator::EQ, teacherId),
					[callbackPtr, isReset](const std::vector<Teacher>& teachers) {
						Json::Value data{};
						if (!teachers.empty()) {
							data["email"] = teachers[0].getValueOfEmail();
						}
						else {
							data["email"] = "";
						}
						data["isReset"] = isReset;
						(*callbackPtr)(getJsonDataResponse(true, data));
						LOG_DEBUG << "Get Settings success";
					},
					[callbackPtr](const DrogonDbException& e) {
						LOG_ERROR << "Database error: " << e.base().what();
						(*callbackPtr)(getJsonDataResponse(false));
					}
				);
			},
			[callbackPtr](const DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false));
			}
		);
	}
	else {
		Json::Value data{};
		(*callbackPtr)(getJsonDataResponse(false));
		LOG_DEBUG << "Invalid request data";
	}
}

void TeacherSettings::getBottomSaying(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Bottom Saying";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<PlatformSettings> mp(dbClientPtr);
	mp.findBy(
		Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "teacher_admin_bottom_saying"),
		[callbackPtr](const std::vector<PlatformSettings>& value){
			Json::Value data{};
			data["saying"] = value[0].getValueOfValue();
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Bottom Saying success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false));
		}
	);
}

void TeacherSettings::adminResetVerify(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Verify Admin Reset Code";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto data = req->getJsonObject();
	if (data) {
		auto verification_code = (*data)["verification_code"].asString();
		auto teacherId = std::to_string(req->attributes()->get<int64_t>("teacherId"));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<PlatformSettings> mp(dbClientPtr);
		mp.findBy(
			Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_reset_verification_code") ||
			Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_password_to_be_reset") ||
			Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "admin_verification_teachers"),
			[callbackPtr, verification_code, teacherId](const std::vector<PlatformSettings>& settings) {
				auto dbClientPtr = drogon::app().getDbClient();
				std::string code = "";
				std::string password = "";
				std::string teachersStr = "";
				for (const auto& setting : settings) {
					if (setting.getValueOfName() == "admin_reset_verification_code")
						code = setting.getValueOfValue();
					if (setting.getValueOfName() == "admin_password_to_be_reset")
						password = setting.getValueOfValue();
					if (setting.getValueOfName() == "admin_verification_teachers")
						teachersStr = setting.getValueOfValue();
				}
				auto teachers = split(teachersStr, ',');
				if (code == verification_code) {
					auto isTeacherVerifyed = std::find(teachers.begin(), teachers.end(), teacherId) != teachers.end();
					if (isTeacherVerifyed) {
						Json::Value data{};
						data["message"] = "该账号已验证";
						(*callbackPtr)(getJsonDataResponse(false, data));
						LOG_DEBUG << "Admin reset Teacher Repeatedly Verify";
						return;
					}
					if (teachers.size() > 1) {
						dbClientPtr->execSqlAsync(
							"UPDATE admin_account SET password = $1 WHERE admin_id = 1;",
							[dbClientPtr](const drogon::orm::Result&) {
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
								dbClientPtr->execSqlAsync(
									sql,
									[](const drogon::orm::Result&) {
										LOG_DEBUG << "Admin reset success";
									},
									[](const drogon::orm::DrogonDbException& e) {
										LOG_ERROR << "Database error: " << e.base().what();
									}
								);
							},
							[](const drogon::orm::DrogonDbException& e) {
								LOG_ERROR << "Database error: " << e.base().what();
							},
							password
						);
					}
					else
					{
						dbClientPtr->execSqlAsync(
							"UPDATE platform_settings SET value = $1 WHERE name = 'admin_verification_teachers'",
							[](const drogon::orm::Result&) {
								LOG_DEBUG << "Admin reset added count";
							},
							[teacherId](const drogon::orm::DrogonDbException& e) {
								LOG_ERROR << "Database error: " << e.base().what();
							},
							teachersStr + (teachers.size() > 0 ? "," : "") + teacherId
						);
					}
					Json::Value data{};
					data["message"] = "验证成功";
					(*callbackPtr)(getJsonDataResponse(true, data));
					LOG_DEBUG << "Admin reset code verified success";
				}
				else {
					Json::Value data{};
					data["message"] = "验证码错误";
					(*callbackPtr)(getJsonDataResponse(false, data));
					LOG_DEBUG << "Admin reset code verification failed";
				}
			},
			[callbackPtr](const DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false));
			}
		);
	}
	else {
		LOG_DEBUG << "Invalid request data";
		(*callbackPtr)(getJsonDataResponse(false));
	}
}
