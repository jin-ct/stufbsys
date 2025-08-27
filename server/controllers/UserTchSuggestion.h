#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/Teacher.h"
#include "../models/TeacherSuggestion.h"
#include "../models/TeacherDataStatistics.h"
#include "../models/VoteRecord.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class UserTchSuggestion : public drogon::HttpController<UserTchSuggestion>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(UserTchSuggestion::getTeacherList, "/teacher/list", Get);
      METHOD_ADD(UserTchSuggestion::getSuggestionlist, "/suggestion/list", Get);
      METHOD_ADD(UserTchSuggestion::addSuggestion, "/suggestion/add", Post, "UserLoginFilter");
      METHOD_ADD(UserTchSuggestion::addVote, "/vote", Get, "UserLoginFilter");
    METHOD_LIST_END

    void getTeacherList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getSuggestionlist(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
