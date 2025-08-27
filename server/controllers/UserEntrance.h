#pragma once

#include <drogon/HttpController.h>
#include "../plugins/JwtPlugin.h"
#include "../models/User.h"
#include "../models/PlatformSettings.h"
#include "../models/EntranceExam.h"
#include "../models/Notice.h"
#include "../utils/utils.h"
#include "../utils/verifyImageComparator.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class UserEntrance : public drogon::HttpController<UserEntrance>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(UserEntrance::getExamQuestions, "/exam/get-questions", Get);
      METHOD_ADD(UserEntrance::login, "/login", Post);
      METHOD_ADD(UserEntrance::registerByExam, "/register", Post);
      METHOD_ADD(UserEntrance::getSettings, "/get-settings", Get);
      METHOD_ADD(UserEntrance::getNotices, "/get-notices", Get);
      METHOD_ADD(UserEntrance::incrementVisitCount, "/increment-visit", Get, "UserLoginFilter");
      METHOD_ADD(UserEntrance::userVerify, "/verify", Post);
    METHOD_LIST_END

    void getExamQuestions(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void registerByExam(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getNotices(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void incrementVisitCount(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void userVerify(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;

    static void handleUnknownUser(const std::string ipAddr,
                          Json::Value reqData,
                          std::shared_ptr<std::function<void (const drogon::HttpResponsePtr &)>> callbackPtr);

    static std::string getToken(const User& user);
};
