#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/TeacherDataStatistics.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class TeacherWelcome : public drogon::HttpController<TeacherWelcome>
{
public:
    METHOD_LIST_BEGIN
      METHOD_ADD(TeacherWelcome::getDatas, "/get", Post, "TeacherLoginFilter");
    METHOD_LIST_END

    void getDatas(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
