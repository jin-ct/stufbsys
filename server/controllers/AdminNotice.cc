#include "AdminNotice.h"

namespace drogon {
	template<>
	inline Notice fromRequest(const HttpRequest& req) {
		auto json = req.getJsonObject();
		auto n = Notice(*json);
		return n;
	}
}

void AdminNotice::getNoticeList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Get Notice List";
	auto data = req->getJsonObject();
	if (data) {
		auto perPage = (*data)["pageSize"].asInt();
		auto page = (*data)["toPage"].asInt();
		auto searchContent = "%" + (*data)["search"]["content"].asString() + "%";
		auto searchDate = (*data)["search"]["date"];
		auto criteria = Criteria(Notice::Cols::_show_text, CompareOperator::Like, searchContent) ||
			Criteria(Notice::Cols::_publisher, CompareOperator::Like, searchContent);
		if (!searchDate[0].asString().empty()) criteria = criteria &&
			Criteria(Notice::Cols::_date, CompareOperator::GE, searchDate[0].asString()) &&
			Criteria(Notice::Cols::_date, CompareOperator::LE, searchDate[1].asString());

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Notice> mp(dbClientPtr);
		auto rowNum = mp.count(criteria);
		mp.orderBy(Notice::Cols::_notice_id, SortOrder::DESC).paginate(page, perPage).findBy(
			criteria,
			[callbackPtr, rowNum](const std::vector<Notice>& notices) {
				Json::Value data{};
				Json::Value list(Json::arrayValue);
				for (const auto& notice : notices) {
					Json::Value noticeJson;
					noticeJson["id"] = notice.getValueOfNoticeId();
					noticeJson["text"] = notice.getValueOfShowText();
					noticeJson["detail"] = notice.getValueOfDetail();
					noticeJson["publisher"] = notice.getValueOfPublisher();
					noticeJson["state"] = notice.getValueOfIsShow();
					noticeJson["date"] = notice.getDate()->toDbStringLocal();
					list.append(noticeJson);
				}
				data["list"] = list;
				data["total"] = rowNum;
				(*callbackPtr)(getJsonDataResponse(true, data));
				LOG_DEBUG << "Get Notice List success";
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

void AdminNotice::addNotice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Notice&& notice) const
{
	LOG_DEBUG << "Add Notice";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Notice> mp(dbClientPtr);
	mp.insert(
		notice,
		[callbackPtr](const Notice& notice) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Add Notice success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminNotice::updateNotice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Notice&& notice) const
{
	LOG_DEBUG << "Update Notice";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto dbClientPtr = drogon::app().getDbClient();
	Mapper<Notice> mp(dbClientPtr);
	mp.update(
		notice,
		[callbackPtr](const std::size_t count) {
			Json::Value data{};
			data["message"] = "成功";
			(*callbackPtr)(getJsonDataResponse(true, data));
			LOG_DEBUG << "Update Notice success";
		},
		[callbackPtr](const DrogonDbException& e) {
			LOG_ERROR << "Database error: " << e.base().what();
			Json::Value data{};
			data["message"] = "系统错误";
			(*callbackPtr)(getJsonDataResponse(false, data));
		}
	);
}

void AdminNotice::deleteNotice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const
{
	LOG_DEBUG << "Delete Notice";
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto json = req->getJsonObject();
	if (json && json->isMember("ids")) {
		std::vector<int64_t> ids;
		for (const auto& id : (*json)["ids"]) {
			ids.push_back(id.asInt64());
		}
		auto dbClientPtr = drogon::app().getDbClient();
		Mapper<Notice> mp(dbClientPtr);
		mp.deleteBy(
			Criteria(Notice::Cols::_notice_id, orm::CompareOperator::In, ids),
			[callbackPtr](const std::size_t count) {
				(*callbackPtr)(getJsonDataResponse(true));
				LOG_DEBUG << "Delete Notice success";
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
