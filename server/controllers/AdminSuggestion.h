#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/SchoolSuggestionLong.h"
#include "../models/SchoolSuggestionShort.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminSuggestion : public drogon::HttpController<AdminSuggestion>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(AdminSuggestion::getShortSuggestionList, "/short/list", Post, "AdminLoginFilter");
      METHOD_ADD(AdminSuggestion::deleteShortSuggestion, "/short/delete", Post, "AdminLoginFilter");
      METHOD_ADD(AdminSuggestion::getLongSuggestionList, "/long/list", Post, "AdminLoginFilter");
      METHOD_ADD(AdminSuggestion::deleteLongSuggestion, "/long/delete", Post, "AdminLoginFilter");
    METHOD_LIST_END

	void getShortSuggestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void deleteShortSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void getLongSuggestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void deleteLongSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
