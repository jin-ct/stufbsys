#include "UserEntrance.h"

void UserEntrance::getExamQuestions(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Exam Questions";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	std::string sql = R"(
		WITH
		choice AS (
		  SELECT * FROM entrance_exam
		  WHERE type = 2
		  ORDER BY RANDOM()
		  LIMIT 2
		),
		judge_true AS (
		  SELECT * FROM entrance_exam
		  WHERE type = 0 AND answer = '对'
		  ORDER BY RANDOM()
		  LIMIT 1
		),
		judge_false AS (
		  SELECT * FROM entrance_exam
		  WHERE type = 0 AND answer = '错'
		  ORDER BY RANDOM()
		  LIMIT 1
		),
		judge_other AS (
		  SELECT * FROM entrance_exam
		  WHERE type = 0
			AND question_id NOT IN (
			  SELECT question_id FROM judge_true
			  UNION
			  SELECT question_id FROM judge_false
			)
		  ORDER BY RANDOM()
		  LIMIT 1
		),
		fill AS (
		  SELECT * FROM entrance_exam
		  WHERE type = 1
		  ORDER BY RANDOM()
		  LIMIT 1
		)
		SELECT * FROM (
		  SELECT * FROM choice
		  UNION ALL
		  SELECT * FROM judge_true
		  UNION ALL
		  SELECT * FROM judge_false
		  UNION ALL
		  SELECT * FROM judge_other
		  UNION ALL
		  SELECT * FROM fill
		) t
		ORDER BY RANDOM();
	)";
	dbClientPtr->execSqlAsync(
		sql,
		[callbackPtr](const drogon::orm::Result& result) {
			if(result.size() < 6) {
				Json::Value data;
				data["message"] = "缺少题目类型或题库为空";
				(*callbackPtr)(getJsonDataResponse(false, data));
				LOG_DEBUG << "No exam questions available";
				return;
			}
			Json::Value data;
			Json::Value judges(Json::arrayValue);
			Json::Value choices(Json::arrayValue);
			for (const auto& row : result) {
				Json::Value questionJson;
				switch(row["type"].as<int>()) {
				case 0:
					questionJson["question"] = row["question"].as<std::string>();
					questionJson["question_id"] = row["question_id"].as<size_t>();
					judges.append(questionJson);
					break;
				case 1:
					questionJson["question"] = row["question"].as<std::string>();
					questionJson["question_id"] = row["question_id"].as<size_t>();
					data["fill"] = questionJson;
					break;
				case 2:
					questionJson["question"] = row["question"].as<std::string>();
					questionJson["question_id"] = row["question_id"].as<size_t>();
					choices.append(questionJson);
					break;
				}
			}
			data["judges"] = judges;
			data["choices"] = choices;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Exam Questions success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false));
		}
	);
}

void UserEntrance::login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "User Entrance Login";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (!json) {
		(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		LOG_DEBUG << "Invalid request data";
		return;
	}
	auto ipAddr = req->getHeader("X-Real-IP");
	auto userIdStr = (*json)["userId"].asString();
	auto fastKey = (*json)["fastKey"].asString();
	if (userIdStr.empty() || fastKey.empty()) {
		handleUnknownUser(ipAddr, *json, callbackPtr);
		return;
	}
	auto userId = std::stoll(userIdStr);
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<User> mp(dbClientPtr);
	mp.findOne(
		Criteria(User::Cols::_user_id, CompareOperator::EQ, userId),
		[callbackPtr, fastKey, ipAddr, json](const User& user) {
			if (decryptText(user.getValueOfFastKey()) == fastKey) {
				auto token = getToken(user);
				Json::Value data;
				data["accessToken"] = token;
				data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt());
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "User Entrance Login success";
			} else {
				LOG_DEBUG << "FastKey error";
				handleUnknownUser(ipAddr, *json, callbackPtr);
			}
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserEntrance::registerByExam(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "User Entrance Register by Exam";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (!json) {
		(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		LOG_DEBUG << "Invalid request data";
		return;
	}
	auto ipAddr = req->getHeader("X-Real-IP");
	auto browserFingerprint = (*json)["browserFingerprint"].asString();
	auto verifyImage_true = (*json)["verifyImage_true"].asString();
	auto verifyImage_false = (*json)["verifyImage_false"].asString();
	auto choices = (*json)["choices"];
	auto judges = (*json)["judges"];
	auto fill = (*json)["fill"];
	if (!(choices.size() == 2) || !(judges.size() == 3)) {
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		LOG_DEBUG << "Invalid request data";
		return;
	}
	std::map<size_t, std::string> answers;
	std::vector<size_t> questionIds;
	for (const auto& choice : choices) {
		auto questionId = choice["question_id"].as<size_t>();
		answers[questionId] = choice["answer"].asString();
		questionIds.push_back(questionId);
	}
	for (const auto& judge : judges) {
		auto questionId = judge["question_id"].as<size_t>();
		answers[questionId] = judge["answer"].asString();
		questionIds.push_back(questionId);
	}
	auto fillId = fill["question_id"].as<size_t>();
	answers[fillId] = fill["answer"].asString();
	questionIds.push_back(fillId);
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<EntranceExam> mp(dbClientPtr);
	mp.findBy(
		Criteria(EntranceExam::Cols::_question_id, CompareOperator::In, questionIds),
		[callbackPtr, answers, browserFingerprint, verifyImage_true, verifyImage_false, ipAddr]
		(const std::vector<EntranceExam>& questions) {
			auto judgesCorrectCount = 0;
			auto choicesAndFillCorrectCount = 0;
			for (const auto& question : questions) {
				auto trueAnswer = question.getValueOfType() == 0 ? question.getValueOfAnswer() : decryptText(question.getValueOfAnswer());
				if (trueAnswer == answers.at(question.getValueOfQuestionId())) {
					switch (question.getValueOfType()) {
					case 0:
						judgesCorrectCount++;
						break;
					case 1:
						choicesAndFillCorrectCount++;
						break;
					case 2:
						choicesAndFillCorrectCount++;
						break;
					}
				}
			}
			if (judgesCorrectCount != 3 || choicesAndFillCorrectCount < 2) {
				Json::Value data;
				data["message"] = "考试未通过";
				(*callbackPtr)(getJsonDataResponse(false, data));
				LOG_DEBUG << "Exam not passed";
				return;
			}
			auto fastKey = getRandomString(64);
			User newUser;
			newUser.setFastKey(encryptText(fastKey));
			newUser.setUniqueIdentification(browserFingerprint);
			newUser.setVerifyImageTrue(verifyImage_true);
			newUser.setVerifyImageFalse(verifyImage_false);
			newUser.setIpAddress(ipAddr);
			auto dbClientPtr = drogon::app().getDbClient();
			Mapper<User> userMp(dbClientPtr);
			userMp.insert(
				newUser,
				[callbackPtr, fastKey](const User& user) {
					Json::Value data;
					data["userId"] = user.getValueOfUserId();
					data["fastKey"] = fastKey;
					(*callbackPtr)(getJsonDataResponse(true, data));
					LOG_DEBUG << "User Entrance Register by Exam success";
				},
				[callbackPtr](const DrogonDbException& e) {
					LOG_ERROR << "Database error: " << e.base().what();
					Json::Value data;
					data["message"] = "系统错误";
					(*callbackPtr)(getJsonDataResponse(false, data, drogon::k500InternalServerError));
				}
			);
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data;
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data, drogon::k500InternalServerError));
		}
	);
}

void UserEntrance::getSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
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

void UserEntrance::getNotices(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Notice> mp(dbClientPtr);
	mp.orderBy(Notice::Cols::_notice_id, SortOrder::DESC).findAll(
		[callbackPtr](const std::vector<Notice>& notices) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& notice : notices) {
				if (notice.getValueOfIsShow()) {
					Json::Value noticeJson;
					noticeJson["id"] = notice.getValueOfNoticeId();
					noticeJson["text"] = notice.getValueOfShowText();
					noticeJson["detail"] = notice.getValueOfDetail();
					noticeJson["publisher"] = notice.getValueOfPublisher();
					noticeJson["date"] = notice.getDate()->toDbStringLocal();
					list.append(noticeJson);
				}
			}
			data["notices"] = list;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Notice List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserEntrance::incrementVisitCount(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	dbDataIncrementOne("data_statistics", "visit");
	dbDataIncrementOne("teacher_data_statistics", "visit");
	callback(getJsonDataResponse(true));
}

void UserEntrance::userVerify(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
	LOG_DEBUG << "User Verify";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (!json || !json->isMember("questions") || !json->isMember("tempToken")) {
		(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		LOG_DEBUG << "Invalid request data";
		return;
	}
	auto verifyImage_1 = (*json)["questions"][0]["verifyImage"].asString();
	auto verifyImage_2 = (*json)["questions"][1]["verifyImage"].asString();
	auto answer_1 = (*json)["questions"][0]["answer"].asBool();
	auto answer_2 = (*json)["questions"][1]["answer"].asBool();
	auto optionsJsonStr = decryptText((*json)["tempToken"].asString());
	if (optionsJsonStr.empty()) {
		(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		LOG_DEBUG << "Temp Token error";
		return;
	}
	auto options = parseJsonString(optionsJsonStr).value();
	auto unknownUserIds = options["unknownUserIds"];
	std::vector<int64_t> ids;
	for (const auto& id : unknownUserIds) {
		ids.push_back(id.as<int64_t>());
	}
	bool isSimpleContrast = false;
	if (ids.size() == 1) {
		isSimpleContrast = (*json)["isSimpleContrast"].asBool();
	}
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<User> mp(dbClientPtr);
	mp.findBy(
		Criteria(User::Cols::_user_id, CompareOperator::In, ids),
		[callbackPtr, isSimpleContrast, verifyImage_1, verifyImage_2, answer_1, answer_2]
		(const std::vector<User>& users) {
			if (users.size() == 1) {
			 	auto verifyImage_1_answer_img = answer_1 ? users[0].getValueOfVerifyImageTrue() : users[0].getValueOfVerifyImageFalse();
			 	auto verifyImage_2_answer_img = answer_2 ? users[0].getValueOfVerifyImageTrue() : users[0].getValueOfVerifyImageFalse();
				if (
					compareImagesBase64(verifyImage_1, verifyImage_1_answer_img, isSimpleContrast).isSame ||
					compareImagesBase64(verifyImage_2, verifyImage_2_answer_img, isSimpleContrast).isSame
				) {
					auto token = getToken(users[0]);
					Json::Value data;
					data["accessToken"] = token;
					data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt());
					data["userId"] = users[0].getValueOfUserId();
					data["fastKey"] = decryptText(users[0].getValueOfFastKey());
					(*callbackPtr)(getJsonDataResponse(true, data));
					LOG_DEBUG << "User Entrance Login success";
					return;
				} else {
					Json::Value data;
					data["to"] = "register";
					(*callbackPtr)(getJsonDataResponse(false, data));
					LOG_DEBUG << "User Unregistered";
					return;
				}

			} else if (users.size() > 1) {
				std::map<int64_t, ImageCompareResult> similaritys;
				for (int64_t i = 0; i < users.size(); ++i) {
					auto verifyImage_1_answer_img = answer_1 ? users[i].getValueOfVerifyImageTrue() : users[i].getValueOfVerifyImageFalse();
					auto verifyImage_2_answer_img = answer_2 ? users[i].getValueOfVerifyImageTrue() : users[i].getValueOfVerifyImageFalse();
					auto img1_compare_res = compareImagesBase64(verifyImage_1, verifyImage_1_answer_img, isSimpleContrast);
					auto img2_compare_res = compareImagesBase64(verifyImage_2, verifyImage_2_answer_img, isSimpleContrast);
					ImageCompareResult m_res = {
						(img1_compare_res.similarity + img2_compare_res.similarity) / 2,
						(img1_compare_res.isSame && img2_compare_res.isSame)
					};
					similaritys.insert(std::make_pair(i, m_res));
				}
				auto maxSimilarityUser = std::max_element(similaritys.begin(), similaritys.end(),
					[](const auto& p1, const auto& p2) { return p1.second.similarity < p2.second.similarity; });
				if (!maxSimilarityUser->second.isSame) {
					Json::Value data;
					data["to"] = "register";
					(*callbackPtr)(getJsonDataResponse(false, data));
					LOG_DEBUG << "User Unregistered";
					return;
				}
				auto token = getToken(users[0]);
				Json::Value data;
				data["accessToken"] = token;
				data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt());
				data["userId"] = users[maxSimilarityUser->first].getValueOfUserId();
				data["fastKey"] = decryptText(users[maxSimilarityUser->first].getValueOfFastKey());
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "User Entrance Login success";
			}
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserEntrance::handleUnknownUser(const std::string ipAddr, Json::Value reqData, std::shared_ptr<std::function<void (const drogon::HttpResponsePtr &)>> callbackPtr)
{
	LOG_DEBUG << "Handle Unknown User";
	auto browserFingerprint = reqData["browserFingerprint"].asString();
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<User> mp(dbClientPtr);
	mp.findBy(
		Criteria(User::Cols::_ip_address, CompareOperator::EQ, ipAddr) ||
		Criteria(User::Cols::_unique_identification, CompareOperator::EQ, browserFingerprint),
		[callbackPtr, ipAddr, browserFingerprint](const std::vector<User>& users) {
			if (users.empty()){
				Json::Value data;
				data["to"] = "register";
				(*callbackPtr)(getJsonDataResponse(false, data));
				LOG_DEBUG << "User Unregistered";
				return;
			}

			std::vector<User> ipAndFingerprintEqUsers;
			std::vector<User> ipEqUsers;
			std::vector<User> fingerprintEqUsers;
			for (const auto &user: users) {
				if (user.getValueOfIpAddress() == ipAddr && user.getValueOfUniqueIdentification() == browserFingerprint) {
					ipAndFingerprintEqUsers.push_back(user);
				} else if (user.getValueOfIpAddress() == ipAddr) {
					ipEqUsers.push_back(user);
				} else if (user.getValueOfUniqueIdentification() == browserFingerprint) {
					fingerprintEqUsers.push_back(user);
				}
			}

			if (ipAndFingerprintEqUsers.size() == 1) {
				auto token = getToken(users[0]);
				Json::Value data;
				data["accessToken"] = token;
				data["expires"] = getExpiresFromCurrentTime(drogon::app().getCustomConfig()["sessionTime"].asInt());
				data["userId"] = users[0].getValueOfUserId();
				data["fastKey"] = decryptText(users[0].getValueOfFastKey());
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "User Entrance Login success";
				return;
			}
			if (ipAndFingerprintEqUsers.size() > 1) {
				LOG_DEBUG << "User Entrance Login ambiguous (ipAndFingerprintEq, multiple)";
				Json::Value data;
				data["to"] = "verify";
				Json::Value options;
				Json::Value unknownUserIds(Json::arrayValue);
				for (const auto &user: ipAndFingerprintEqUsers) {
					Json::Value userJson;
					unknownUserIds.append(user.getValueOfUserId());
				}
				options["unknownUserIds"] = unknownUserIds;
				data["tempToken"] = encryptText(options.toStyledString());
				(*callbackPtr)(getJsonDataResponse(false, data));
				return;
			}
			if (ipEqUsers.size() == 1 || fingerprintEqUsers.size() == 1) {
				LOG_DEBUG << "User Entrance Login ambiguous (" << (ipEqUsers.size() == 1 ? "ipEqCount" : "fingerprintEqCount") << ", single)";
				Json::Value data;
				data["to"] = "verify";
				Json::Value options;
				Json::Value unknownUserIds(Json::arrayValue);
				unknownUserIds.append(ipEqUsers.size() == 1 ? ipEqUsers[0].getValueOfUserId() : fingerprintEqUsers[0].getValueOfUserId());
				options["unknownUserIds"] = unknownUserIds;
				options["isSimpleContrast"] = ipEqUsers.size() == 1 ? false : true;
				data["tempToken"] = encryptText(options.toStyledString());
				(*callbackPtr)(getJsonDataResponse(false, data));
				return;
			}
			if (ipEqUsers.size() > 1 || fingerprintEqUsers.size() > 1) {
				LOG_DEBUG << "User Entrance Login ambiguous (" << (ipEqUsers.size() > 1 ? "ipEqCount" : "fingerprintEqCount") << ", multiple)";
				Json::Value data;
				data["to"] = "verify";
				Json::Value options;
				Json::Value unknownUserIds(Json::arrayValue);
				for (const auto &user: (ipEqUsers.size() > 1 ? ipEqUsers : fingerprintEqUsers)) {
					Json::Value userJson;
					unknownUserIds.append(user.getValueOfUserId());
				}
				options["unknownUserIds"] = unknownUserIds;
				data["tempToken"] = encryptText(options.toStyledString());
				(*callbackPtr)(getJsonDataResponse(false, data));
			}
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

std::string UserEntrance::getToken(const User& user)
{
	auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
	auto jwt = jwtPtr->init();
	auto token = jwt.encode("user_id", *(user.getUserId()));
	return token;
}
