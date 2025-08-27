#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/DataStatistics.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminWelcome : public drogon::HttpController<AdminWelcome>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(AdminWelcome::getDatas, "/get", Get, "AdminLoginFilter");
    METHOD_LIST_END

    void getDatas(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
