#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/SchoolSuggestionLong.h"
#include "../models/SchoolSuggestionShort.h"
#include "../models/VoteRecord.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class UserSuggestion : public drogon::HttpController<UserSuggestion>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(UserSuggestion::getShortSuggestionlist, "/short/list", Get);
      METHOD_ADD(UserSuggestion::addShortSuggestion, "/short/add", Post, "UserLoginFilter");
      METHOD_ADD(UserSuggestion::getLongSuggestionlist, "/long/list", Get);
      METHOD_ADD(UserSuggestion::addLongSuggestion, "/long/add", Post, "UserLoginFilter");
      METHOD_ADD(UserSuggestion::addVote, "/vote", Get, "UserLoginFilter");
    METHOD_LIST_END

    void getShortSuggestionlist(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addShortSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getLongSuggestionlist(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addLongSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;

    static void updateAiScore(const std::string& title, const std::string& content, const short& type, const int64_t& sug_id, const int& tryCount = 0);
};
