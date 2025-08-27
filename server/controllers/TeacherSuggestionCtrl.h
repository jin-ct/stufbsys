#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/TeacherSuggestion.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class TeacherSuggestionCtrl : public drogon::HttpController<TeacherSuggestionCtrl>
{
public:
    METHOD_LIST_BEGIN
      METHOD_ADD(TeacherSuggestionCtrl::getSuggestionList, "/list", Post, "TeacherLoginFilter");
      METHOD_ADD(TeacherSuggestionCtrl::deleteSuggestion, "/delete", Post, "TeacherLoginFilter");
    METHOD_LIST_END

    void getSuggestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void deleteSuggestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
