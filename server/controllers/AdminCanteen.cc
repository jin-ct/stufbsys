#include "AdminCanteen.h"

namespace drogon {
	template<>
	inline Dish fromRequest(const HttpRequest& req) {
		auto json = req.getJsonObject();
		auto dish = Dish(*json);
		return dish;
	}
}

void AdminCanteen::getDishList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Dish List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto sortByScore = (*data)["sortByScore"];
		auto searchName = "%" + (*data)["search"]["dishname"].asString() + "%";
		auto searchDate = (*data)["search"]["date"];
		auto criteria = Criteria(Dish::Cols::_name, CompareOperator::Like, searchName);
		if (!searchDate[0].asString().empty()) criteria = criteria &&
			Criteria(Dish::Cols::_date, CompareOperator::GE, searchDate[0].asString()) &&
			Criteria(Dish::Cols::_date, CompareOperator::LE, searchDate[1].asString());

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Dish> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);

		if (!sortByScore.empty()) {
			auto sortByScoreStr = sortByScore.asString();
			if (!sortByScoreStr.empty()) {
				if (sortByScoreStr == "asc") mp = mp.orderBy(Dish::Cols::_avg_score, SortOrder::ASC);
				else mp = mp.orderBy(Dish::Cols::_avg_score, SortOrder::DESC);
			}
		}
		mp.orderBy(Dish::Cols::_dish_id, SortOrder::ASC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<Dish>& dishes) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& dish : dishes) {
					Json::Value dishJson;
					dishJson["dish_id"] = dish.getValueOfDishId();
					dishJson["name"] = dish.getValueOfName();
					dishJson["description"] = dish.getValueOfDescription();
					dishJson["lastest_evaluate"] = dish.getValueOfLastestEvaluate();
					dishJson["evaluate_count"] = dish.getValueOfEvaluateCount();
					dishJson["avg_score"] = dish.getValueOfAvgScore();
					dishJson["date"] = dish.getDate()->toDbStringLocal();
					list.append(dishJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Dish List success";
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

void AdminCanteen::addDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Dish&& dish) const
{
	LOG_DEBUG << "Add Dish";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Dish> mp(dbClientPtr);
	mp.insert(
		dish,
		[callbackPtr](const Dish& dish) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Add Dish success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR <<"Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminCanteen::updateDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Dish&& dish) const
{
	LOG_DEBUG << "Update Dish";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Dish> mp(dbClientPtr);
	mp.update(
		dish,
		[callbackPtr](const std::size_t count) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Update Dish success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminCanteen::deleteDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Dish";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Dish> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(Dish::Cols::_dish_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Dish success";
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

void AdminCanteen::importDishes(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Import Dishes";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto json = req->getJsonObject();
	if (json) {
		std::string rows;
		auto rowsData = (*json)["data"];
		auto isCover = (*json)["isCover"].asBool();
		std::string sql = "INSERT INTO dish (name, description, date) VALUES ";
		for (const auto& row : rowsData) {
			sql += "('" + row["name"].asString() + "', '" +
				row["description"].asString() + "', '" +
				row["date"].asString() + "'),";
		}
		sql.pop_back();
		sql += ";";
		auto dbClientPtr = drogon::app().getDbClient();
		if (isCover) {
			auto transPtr = dbClientPtr->newTransaction();
			transPtr->execSqlAsync(
				"TRUNCATE TABLE dish RESTART IDENTITY;",
				[transPtr, sql, callbackPtr](const drogon::orm::Result&) {
					transPtr->execSqlAsync(
						sql,
						[callbackPtr](const drogon::orm::Result&) {
							Json::Value data;
							data["message"] = "成功";
							(*callbackPtr)(getJsonDataResponse(true, data));
							LOG_DEBUG << "Import Dishes success";
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
					LOG_DEBUG << "Import Dishes success";
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
		return;
	}
}

void AdminCanteen::getAllEvaluate(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get All Evaluate";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();

	std::string sql = R"(
        SELECT d.dish_id, d.name, d.avg_score, d.evaluate_count, d.date,
               e.evaluate_id, e.score, e.vote_count, e.evaluate_text, e.date AS evaluate_date
        FROM dish d
        LEFT JOIN dish_evaluate e ON d.dish_id = e.dish_id AND e.evaluate_text IS NOT NULL AND e.evaluate_text <> ''
        WHERE d.evaluate_count > 0
        ORDER BY d.dish_id, e.vote_count DESC
    )";

	dbClientPtr->execSqlAsync(
		sql,
		[callbackPtr](const drogon::orm::Result& result) {
			Json::Value data(Json::arrayValue);
			std::map<int64_t, Json::Value> dishMap;

			for (const auto& row : result) {
				int64_t dishId = row["dish_id"].as<int64_t>();
				if (dishMap.find(dishId) == dishMap.end()) {
					Json::Value dishJson;
					dishJson["name"] = row["name"].as<std::string>();
					dishJson["avg_score"] = row["avg_score"].isNull() ? 0.0 : row["avg_score"].as<float>();
					dishJson["evaluate_count"] = row["evaluate_count"].isNull() ? 0 : row["evaluate_count"].as<int32_t>();
					dishJson["date"] = row["date"].isNull() ? "" : row["date"].as<std::string>();
					dishJson["evaluates"] = Json::arrayValue;
					dishMap[dishId] = dishJson;
				}
				if (!row["evaluate_id"].isNull()) {
					Json::Value evalJson;
					evalJson["id"] = row["evaluate_id"].as<int64_t>();
					evalJson["score"] = row["score"].isNull() ? 0.0 : row["score"].as<float>();
					evalJson["vote_count"] = row["vote_count"].isNull() ? 0 : row["vote_count"].as<int32_t>();
					evalJson["suggestion"] = row["evaluate_text"].isNull() ? "" : row["evaluate_text"].as<std::string>();
					evalJson["date"] = row["evaluate_date"].isNull() ? "" : row["evaluate_date"].as<std::string>();
					dishMap[dishId]["evaluates"].append(evalJson);
				}
			}
			for (auto& [_, dishJson] : dishMap) {
				data.append(dishJson);
			}
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Get All Evaluate success";
		},
		[callbackPtr](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			(*callbackPtr)(getJsonDataResponse(false));
		}
	);
}

void AdminCanteen::getEvaluateList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Evaluate List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto dishId = (*data)["dish_id"].asInt64();
		auto sortByScore = (*data)["sortByScore"];
		auto isNotSuggestionNull = (*data)["isNotSuggestionNull"].asBool();
		auto criteria = Criteria(DishEvaluate::Cols::_dish_id, CompareOperator::EQ, dishId);
		if (isNotSuggestionNull) criteria = criteria && Criteria(DishEvaluate::Cols::_evaluate_text, CompareOperator::IsNotNull);

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<DishEvaluate> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);

		if (!sortByScore.empty()) {
			auto sortByScoreStr = sortByScore.asString();
			if (!sortByScoreStr.empty()) {
				if (sortByScoreStr == "asc") mp = mp.orderBy(DishEvaluate::Cols::_score, SortOrder::ASC);
				else mp = mp.orderBy(DishEvaluate::Cols::_score, SortOrder::DESC);
			}
		}
		mp.orderBy(DishEvaluate::Cols::_evaluate_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<DishEvaluate>& evaluates) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& evaluate : evaluates) {
					Json::Value evalJson;
					evalJson["id"] = evaluate.getValueOfEvaluateId();
					evalJson["score"] = evaluate.getValueOfScore();
					evalJson["vote_count"] = evaluate.getValueOfVoteCount();
					evalJson["suggestion"] = evaluate.getValueOfEvaluateText();
					evalJson["date"] = evaluate.getDate()->toDbStringLocal();
					list.append(evalJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Evaluate List success";
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

void AdminCanteen::deleteEvaluate(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Evaluate";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<DishEvaluate> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(DishEvaluate::Cols::_evaluate_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Evaluate success";
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

void AdminCanteen::getDishAddList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get DishAdd List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto sortByScore = (*data)["sortByScore"];
		auto sortByVote = (*data)["sortByVote"];
		auto searchName = "%" + (*data)["search"]["dishname"].asString() + "%";
		auto searchDate = (*data)["search"]["date"];
		auto criteria = Criteria(DishAdd::Cols::_name, CompareOperator::Like, searchName);
		if (!searchDate[0].asString().empty()) criteria = criteria &&
			Criteria(DishAdd::Cols::_date, CompareOperator::GE, searchDate[0].asString()) &&
			Criteria(DishAdd::Cols::_date, CompareOperator::LE, searchDate[1].asString());

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<DishAdd> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);

		if (!sortByScore.empty()) {
			auto sortByScoreStr = sortByScore.asString();
			if (!sortByScoreStr.empty()) {
				if (sortByScoreStr == "asc") mp = mp.orderBy(DishAdd::Cols::_ai_score, SortOrder::ASC);
				else mp = mp.orderBy(DishAdd::Cols::_ai_score, SortOrder::DESC);
			}
		}
		if (!sortByVote.empty()) {
			auto sortByVoteStr = sortByVote.asString();
			if (!sortByVoteStr.empty()) {
				if (sortByVoteStr == "asc") mp = mp.orderBy(DishAdd::Cols::_vote_count, SortOrder::ASC);
				else mp = mp.orderBy(DishAdd::Cols::_vote_count, SortOrder::DESC);
			}
		}
		mp.orderBy(DishAdd::Cols::_add_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<DishAdd>& dishes) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& dish : dishes) {
					Json::Value dishJson;
					dishJson["id"] = dish.getValueOfAddId();
					dishJson["name"] = dish.getValueOfName();
					dishJson["description"] = dish.getValueOfDescription();
					dishJson["vote"] = dish.getValueOfVoteCount();
					dishJson["score"] = dish.getValueOfAiScore();
					dishJson["states"] = dish.getValueOfState();
					dishJson["date"] = dish.getDate()->toDbStringLocal();
					list.append(dishJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get DishAdd success";
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

void AdminCanteen::deleteDishAdd(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete DishAdd";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<DishAdd> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(DishAdd::Cols::_add_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete DishAdd success";
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

void AdminCanteen::approveDishAdd(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Approve DishAdd";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("id")) {
		auto id = (*json)["id"].asInt64();
		auto dbClientPtr = drogon::app().getDbClient();
		DishAdd newData;
		newData.setAddId(id);
		newData.setState(1); // 1: 已通过（公开审核）

		Mapper<DishAdd> mp(dbClientPtr);
		mp.update(
			newData,
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Approve DishAdd success";
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
