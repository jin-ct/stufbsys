#include "UserCanteen.h"

void UserCanteen::getDishList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Dish List";
	auto searchContent = "%" + req->getOptionalParameter<std::string>("search").value_or("") + "%";
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Dish> mp(dbClientPtr);
	auto criteria = Criteria(Dish::Cols::_name, CompareOperator::Like, searchContent) ||
					Criteria("to_char(\"date\", 'YYYY-MM-DD') like $?"_sql, searchContent);
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(Dish::Cols::_date, SortOrder::DESC).paginate(page, pageSize).findBy(
		criteria,
		[callbackPtr, total](const std::vector<Dish>& dishes) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& dish : dishes) {
				Json::Value dishJson;
				dishJson["id"] = dish.getValueOfDishId();
				dishJson["name"] = dish.getValueOfName();
				dishJson["avg_score"] = dish.getValueOfAvgScore();
				dishJson["date"] = dish.getDate()->toDbStringLocal();
				list.append(dishJson);
			}
			data["dishes"] = list;
			if(total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Dish List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserCanteen::evaluateDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Evaluate Dish";
	auto json = req->getJsonObject();
	if (!json) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto dishId = (*json)["id"].asInt64();
	auto score = (*json)["score"].asInt();
	auto suggestion = (*json)["suggestion"].asString();
	if (dishId <= 0 || score < 1 || score > 5) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto userId = req->attributes()->get<int64_t>("userId");
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<DishEvaluate> mp(dbClientPtr);
	mp.findBy(
		Criteria(DishEvaluate::Cols::_user_id, CompareOperator::EQ, userId) &&
		Criteria(DishEvaluate::Cols::_dish_id, CompareOperator::EQ, dishId),
		[callbackPtr, dishId, userId, score, suggestion](const std::vector<DishEvaluate>& searchRes) {
			if (searchRes.size() > 0) {
				LOG_DEBUG << "Repeated Evaluate by the user";
				Json::Value res;
				res["message"] = "你已评价过该菜品";
				(*callbackPtr)(getJsonDataResponse(false, res, drogon::k400BadRequest));
				return;
			}
			auto dbClientPtr = drogon::app().getDbClient();
			Mapper<DishEvaluate> mp(dbClientPtr);
			DishEvaluate evaluate;
			evaluate.setDishId(dishId);
			evaluate.setUserId(userId);
			evaluate.setScore(score);
			evaluate.setEvaluateText(suggestion);
			mp.insert(
				evaluate,
				[callbackPtr](const DishEvaluate& e) {
					std::string sql = R"(
						UPDATE "dish"
						SET
							"avg_score" = ROUND((
								SELECT AVG("score") FROM "dish_evaluate" WHERE dish_id = $2
							)::numeric, 2),
							"lastest_evaluate" = $1,
							"evaluate_count" = "evaluate_count" + 1
						WHERE "dish_id" = $2;
					)";
					auto dbClientPtr = drogon::app().getDbClient();
					dbClientPtr->execSqlAsync(
						sql,
						[callbackPtr](const drogon::orm::Result&) {
							LOG_DEBUG << "Update Dish Evaluation Data success";
						},
						[callbackPtr](const drogon::orm::DrogonDbException& e) {
							LOG_ERROR << "Database error: " << e.base().what();
						},
						e.getValueOfEvaluateText(),
						e.getValueOfDishId()
					);
					(*callbackPtr)(getJsonDataResponse(true));
					dbDataIncrementOne("data_statistics", "evaluate_num");
					LOG_DEBUG << "Dish Evaluation Add success";
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

void UserCanteen::getEvaluateList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Evaluate List";
	auto dishId = req->getOptionalParameter<int64_t>("dishId").value_or(0);
	if (dishId <= 0) {
		LOG_DEBUG << "Invalid dish ID";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<DishEvaluate> mp(dbClientPtr);
	auto criteria = Criteria(DishEvaluate::Cols::_dish_id, CompareOperator::EQ, dishId) &&
		            Criteria(DishEvaluate::Cols::_evaluate_text, CompareOperator::IsNotNull) &&
					Criteria(DishEvaluate::Cols::_evaluate_text, CompareOperator::NE, "");
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(DishEvaluate::Cols::_date, SortOrder::DESC)
	  .orderBy(DishEvaluate::Cols::_evaluate_id, SortOrder::DESC)
	  .paginate(page, pageSize)
	  .findBy(
		criteria,
		[callbackPtr, total](const std::vector<DishEvaluate>& evaluates) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& evaluate : evaluates) {
				Json::Value evaluateJson;
				evaluateJson["id"] = evaluate.getValueOfEvaluateId();
				evaluateJson["score"] = evaluate.getValueOfScore();
				evaluateJson["suggestion"] = evaluate.getValueOfEvaluateText();
				evaluateJson["vote"] = evaluate.getValueOfVoteCount();
				evaluateJson["date"] = evaluate.getDate()->toDbStringLocal();
				list.append(evaluateJson);
			}
			data["evaluates"] = list;
			if (total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Evaluate List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserCanteen::getDishAddList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Dish Add List";
	auto pageSize = req->getOptionalParameter<int>("pageSize").value_or(20);
	auto page = req->getOptionalParameter<int>("page").value_or(1);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<DishAdd> mp(dbClientPtr);
	auto criteria = Criteria(DishAdd::Cols::_state, CompareOperator::EQ, int16_t(1));
	size_t total = -1;
	if (page == 1) {
		total = mp.count(criteria);
	}
	mp.orderBy(DishAdd::Cols::_ai_score, SortOrder::DESC)
      .orderBy(DishAdd::Cols::_add_id, SortOrder::DESC)
	  .paginate(page, pageSize)
	  .findBy(
		criteria,
		[callbackPtr, total](const std::vector<DishAdd>& dishes) {
			Json::Value data{};
			Json::Value list(Json::arrayValue);
			for (const auto& dish : dishes) {
				Json::Value dishJson;
				dishJson["id"] = dish.getValueOfAddId();
				dishJson["name"] = dish.getValueOfName();
				dishJson["score"] = dish.getValueOfAiScore();
				dishJson["description"] = dish.getValueOfDescription();
				dishJson["vote"] = dish.getValueOfVoteCount();
				dishJson["date"] = dish.getDate()->toDbStringLocal();
				list.append(dishJson);
			}
			data["dishes"] = list;
			if (total != -1) data["total"] = total;
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get Dish Add List success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserCanteen::addDishAdd(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Add DishAdd";
	auto json = req->getJsonObject();
	if (!json) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto name = (*json)["name"].asString();
	auto description = (*json)["description"].asString();
	if (name.empty() || description.empty()) {
		LOG_DEBUG << "Invalid request data";
		callback(getJsonDataResponse(false, Json::Value(), drogon::k400BadRequest));
		return;
	}
	auto userId = req->attributes()->get<int64_t>("userId");
	DishAdd dishAdd;
	dishAdd.setName(name);
	dishAdd.setDescription(description);
	dishAdd.setUserId(userId);
	dishAdd.setState(0);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<DishAdd> mp(dbClientPtr);
	mp.insert(
		dishAdd,
		[callbackPtr](const DishAdd& dish) {
			(*callbackPtr)(getJsonDataResponse(true));
			dbDataIncrementOne("data_statistics", "dish_add_num");
			updateDishAddAiScore(dish);
			LOG_DEBUG << "Add DishAdd success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
		}
	);
}

void UserCanteen::addVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
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
				res["message"] = "你已投过票";
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
				[callbackPtr, objectId](const VoteRecord& v) {
					(*callbackPtr)(getJsonDataResponse(true));
					switch (v.getValueOfType()) {
					case 0:
						dbDataIncrementOne("dish_evaluate", "vote_count", false, "evaluate_id", objectId);
						break;
					case 1:
						dbDataIncrementOne("dish_add", "vote_count", false, "add_id", objectId);
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

void UserCanteen::updateDishAddAiScore(const DishAdd& dish, const int& tryCount)
{
	Json::Value messages(Json::arrayValue);
	Json::Value message_sys;
	Json::Value message_user;
	message_sys["role"] = "system";
	message_sys["content"] = R"(你需要对用户描述的菜品进行打分(1至5分，两位小数)，你可以根据以下几方面进行综合评估：
								  1、可行性，即菜品是否常见(可通过菜品名称、描述进行判断)或是否可以根据其描述来制作；
								  2、确定性，即菜品是否有明确的名称和描述，描述是否能准确体现出口味、烹饪方式、原料等特征；
								  3、描述的完整性，即菜品描述是否详尽，是否包含了足够的信息来判断菜品的特点和口味；
								  4、适用性，即是否适合于在学校食堂制作，并符合大多数学生口味；
								  5、健康性，即菜品是否符合健康饮食的标准，是否包含过多的油脂、糖分等不健康成分；
								  6、营养性，即菜品是否包含足够的营养成分，是否能满足学生的日常营养需求；
								请使用{"score":number}的json格式回答
								)";
	message_user["role"] = "user";
	message_user["content"] = "名称：" + dish.getValueOfName() + " | 描述：" + dish.getValueOfDescription();
	messages.append(message_sys);
	messages.append(message_user);
	requestAiApi(
		messages,
		[dish, tryCount](const Json::Value res) {
			if (res["success"].asBool() && res["data"]["choices"].size() > 0) {
				auto content = parseJsonString(res["data"]["choices"][0]["message"]["content"].asString());
				if (!content.has_value() || !content.value().isMember("score")) {
					if (tryCount < 3) {
						LOG_ERROR << "Ai Out Data error，retrying(" + std::to_string(tryCount) + ")...";
						drogon::app().getLoop()->runAfter(10.0, [dish, tryCount]() {
							UserCanteen::updateDishAddAiScore(dish, tryCount + 1);
						});
					}
					return;
				}
				auto score = content.value()["score"].asDouble();

				auto dbClientPtr = drogon::app().getDbClient();
				Mapper<DishAdd> mp(dbClientPtr);
				std::vector<std::string> col;
				col.push_back("ai_score");
				mp.updateBy(
					col,
					[](const std::size_t count) {
						if (count > 0) LOG_DEBUG << "Update Dish Add Ai Score success";
					},
					[](const DrogonDbException& e) {
						LOG_ERROR << "Database error: " << e.base().what();
					},
					Criteria(DishAdd::Cols::_add_id, CompareOperator::EQ, dish.getValueOfAddId()),
					score
				);
			}
			else {
				LOG_ERROR << "Update Dish Add Ai Score error";
			}
		}
	);
}
