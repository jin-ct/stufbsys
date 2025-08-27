#pragma once

#include <drogon/HttpController.h>
#include "../models/Notice.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminNotice : public drogon::HttpController<AdminNotice>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(AdminNotice::getNoticeList, "/list", Post, "AdminLoginFilter");
      METHOD_ADD(AdminNotice::addNotice, "/add", Post, "AdminLoginFilter");
      METHOD_ADD(AdminNotice::updateNotice, "/update", Post, "AdminLoginFilter");
      METHOD_ADD(AdminNotice::deleteNotice, "/delete", Post, "AdminLoginFilter");
    METHOD_LIST_END

    void getNoticeList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addNotice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Notice&& notice) const;
    void updateNotice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Notice&& notice) const;
    void deleteNotice(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
