#include "AdminPlatform.h"

namespace drogon {
	template<>
	inline EntranceExam fromRequest(const HttpRequest& req) {
		auto json = req.getJsonObject();
		auto q = EntranceExam(*json);
		return q;
	}
}

void AdminPlatform::getQuestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Entrance Exam Question List";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto searchQuestion = "%" + (*data)["search"]["question"].asString() + "%";
		auto searchType = (*data)["search"]["type"].asString();
		auto criteria = Criteria(EntranceExam::Cols::_question, CompareOperator::Like, searchQuestion);
		if (searchType != "-1") criteria = criteria && Criteria(EntranceExam::Cols::_type, CompareOperator::EQ, searchType);

		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<EntranceExam> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);
		mp.orderBy(EntranceExam::Cols::_question_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<EntranceExam>& questions) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& question : questions) {
					Json::Value questionJson;
					questionJson["id"] = question.getValueOfQuestionId();
					questionJson["question"] = question.getValueOfQuestion();
					questionJson["type"] = question.getValueOfType();
					auto answer = question.getValueOfType() == 0 ? question.getValueOfAnswer() : decryptText(question.getValueOfAnswer());
					questionJson["answer"] = answer;
					list.append(questionJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Entrance Exam Question List success";
			},
			[callbackPtr](const DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
			}
		);
	}
	else {
		LOG_DEBUG << "Invalid request data";
		(*callbackPtr)(getJsonDataResponse(false));
	}
}

void AdminPlatform::addQuestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, EntranceExam&& question) const
{
	LOG_DEBUG << "Add Entrance Exam Question";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	auto answer = question.getValueOfType() == 0 ? question.getValueOfAnswer() : encryptText(question.getValueOfAnswer());
	question.setAnswer(answer);
	Mapper<EntranceExam> mp(dbClientPtr);
	mp.insert(
		question,
		[callbackPtr](const EntranceExam& q) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Add Entrance Exam Question success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminPlatform::updateQuestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, EntranceExam&& question) const
{
	LOG_DEBUG << "Update Entrance Exam Question";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	auto answer = question.getValueOfType() == 0 ? question.getValueOfAnswer() : encryptText(question.getValueOfAnswer());
	question.setAnswer(answer);
	Mapper<EntranceExam> mp(dbClientPtr);
	mp.update(
		question,
		[callbackPtr](const std::size_t count) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Update Entrance Exam Question success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminPlatform::deleteQuestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Entrance Exam Question";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<EntranceExam> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(EntranceExam::Cols::_question_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Entrance Exam Question success";
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

void AdminPlatform::setSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Set Platform Settings";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json && json->isMember("settings")) {
		auto settingsJson = (*json)["settings"];
		if (settingsJson.isMember("ai_api_key") && settingsJson["ai_api_key"].isString()) {
			settingsJson["ai_api_key"] = encryptText(settingsJson["ai_api_key"].asString());
		}
		std::string sql = "UPDATE platform_settings SET value = CASE name ";
		for (const auto& setting : settingsJson.getMemberNames()) {
			sql += "WHEN '" + setting + "' THEN '" + settingsJson[setting].asString() + "' ";
		}
		sql += "END WHERE name IN (";
		for (const auto& setting : settingsJson.getMemberNames()) {
			sql += "'" + setting + "', ";
		}
		sql = sql.substr(0, sql.size() - 2) + ")";
		auto dbClientPtr = drogon::app().getDbClient();
		dbClientPtr->execSqlAsync(
			sql,
			[callbackPtr](const drogon::orm::Result&) {
				Json::Value data{};
				data["message"] = "设置成功";
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Set Platform Settings success";
			},
			[callbackPtr](const drogon::orm::DrogonDbException& e) {
				LOG_ERROR << "Database error: " << e.base().what();
				Json::Value data{};
				data["message"] = "系统错误";
				(*callbackPtr)(getJsonDataResponse(false, data));
			}
		);
	}
	else {
		LOG_DEBUG << "Invalid request data";
		Json::Value data{};
		data["message"] = "数据错误";
		(*callbackPtr)(getJsonDataResponse(false, data));
	}
}

void AdminPlatform::getSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Platform Settings";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<PlatformSettings> mp(dbClientPtr);
	mp.findBy(
		Criteria(PlatformSettings::Cols::_name, CompareOperator::NE, "ai_api_key"),
		[callbackPtr](const std::vector<PlatformSettings>& settings) {
			Json::Value data{};
			for (const auto& setting : settings) {
				Json::Value settingValue(Json::stringValue);
				settingValue = setting.getValueOfValue();
				data[setting.getValueOfName()] = settingValue;
			}
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Platform Settings success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false));
		}
	);
}

void AdminPlatform::setAdminAccountPassword(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Set Admin Account Password";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto data = req->getJsonObject();
	if (data) {
		auto type = (*data)["type"].asInt();
		auto password = (*data)["password"].asString();
		AdminAccount account;
		account.setAdminId(type + 1);
		account.setPassword(BCrypt::generateHash(password));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<AdminAccount> mp(dbClientPtr);
		mp.update(
			account,
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Set Admin Account Password success";
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

void AdminPlatform::uploadLogo(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Upload Platform Logo";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	MultiPartParser fileUpload;
	int parseResult = fileUpload.parse(req);

	if (parseResult != 0) {
		LOG_DEBUG << "Failed to parse uploaded file";
		Json::Value data{};
		data["message"] = "文件解析失败";
		(*callbackPtr)(getJsonDataResponse(false, data));
		return;
	}

	auto& file = fileUpload.getFiles()[0];

	std::string ext = file.getFileExtension().data();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	
	if ((ext != "jpg" && ext != "jpeg" && ext != "png" && ext != "ico") || file.getFileType() != drogon::FT_IMAGE) {
		LOG_DEBUG << "Invalid file type";
		Json::Value data{};
		data["message"] = "文件类型不支持";
		(*callbackPtr)(getJsonDataResponse(false, data));
		return;
	}

	std::filesystem::remove_all("./public/assets/logo");
	std::filesystem::create_directories("./public/assets/logo");
	std::string logoPath = "./public/assets/logo/logo." + ext;

	std::ofstream outFile(logoPath, std::ios::binary | std::ios::out | std::ios::trunc);
	if (!outFile) {
		LOG_ERROR << "Failed to open file for writing";
		Json::Value data{};
		data["message"] = "系统错误";
		(*callbackPtr)(getJsonDataResponse(false, data));
		return;
	}
	outFile.write(file.fileData(), file.fileLength());
	outFile.close();

	Json::Value data{};
	data["url"] = logoPath.substr(8);
	(*callbackPtr)(getJsonDataResponse(true, data));
	LOG_DEBUG << "Logo uploaded successfully";
}

void AdminPlatform::uploadHomeImage(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Upload Home Image";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	MultiPartParser fileUpload;
	int parseResult = fileUpload.parse(req);

	if (parseResult != 0) {
		LOG_DEBUG << "Failed to parse uploaded file";
		Json::Value data{};
		data["message"] = "文件解析失败";
		(*callbackPtr)(getJsonDataResponse(false, data));
		return;
	}

	auto& file = fileUpload.getFiles()[0];

	std::string ext = file.getFileExtension().data();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if ((ext != "jpg" && ext != "jpeg" && ext != "png" && ext != "svg") || file.getFileType() != drogon::FT_IMAGE) {
		LOG_DEBUG << "Invalid file type";
		Json::Value data{};
		data["message"] = "文件类型不支持";
		(*callbackPtr)(getJsonDataResponse(false, data));
		return;
	}

	std::filesystem::remove_all("./public/assets/home_image");
	std::filesystem::create_directories("./public/assets/home_image");
	std::string homeImagePath = "./public/assets/home_image/home_image." + ext;

	std::ofstream outFile(homeImagePath, std::ios::binary | std::ios::out | std::ios::trunc);
	if (!outFile) {
		LOG_ERROR << "Failed to open file for writing";
		Json::Value data{};
		data["message"] = "系统错误";
		(*callbackPtr)(getJsonDataResponse(false, data));
		return;
	}
	outFile.write(file.fileData(), file.fileLength());
	outFile.close();

	Json::Value data{};
	data["url"] = homeImagePath.substr(8);
	(*callbackPtr)(getJsonDataResponse(true, data));
	LOG_DEBUG << "Home image uploaded successfully";
}
