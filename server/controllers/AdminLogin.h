#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/AdminAccount.h"
#include "../models/PlatformSettings.h"
#include "third_party/libbcrypt/include/bcrypt/BCrypt.hpp"
#include "../plugins/JwtPlugin.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminLogin : public drogon::HttpController<AdminLogin>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(AdminLogin::login, "/login", Post);
      METHOD_ADD(AdminLogin::refreshTokenApi, "/refresh-token", Post);
      METHOD_ADD(AdminLogin::getAsyncRoutes, "/get-async-routes", Get);
      METHOD_ADD(AdminLogin::getAdminResetVerificationInf, "/get-admin-reset-verification-inf", Get);
      METHOD_ADD(AdminLogin::adminReset, "/admin-reset", Post);
    METHOD_LIST_END
    
    struct User {
        std::string username;
        std::string password;
    };

    void login(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, User&& pNewUser) const;
    void refreshTokenApi(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getAsyncRoutes(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getAdminResetVerificationInf(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void adminReset(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;

    static std::string getToken(const AdminAccount& user);
    static void StartAdminResetTimeOut();

  private:
	  std::string canteenRouterJsonStr =
          R"({ "path": "/canteen",
              "meta": {
                "title": "饭堂管理",
                "icon": "ep:food",
              },
              "children": [
                {
                  "path": "/canteen/list/index",
                  "name": "Dishlist",
                  "meta": {
                    "title": "菜品列表",
                  }
                },
                {
                  "path": "/canteen/evaluate/index",
                  "name": "Evaluate",
                  "meta": {
                    "title": "评价列表"
                  }
                },
                {
                  "path": "/canteen/add/index",
                  "name": "AddDish",
                  "meta": {
                    "title": "加菜审批"
                  }
                }
              ]})";
      std::string suggestionsRouterJsonStr =
          R"({ "path": "/suggestions",
              "meta": {
                "title": "建议管理",
                "icon": "ep:chat-dot-square",
              },
              "children": [
                {
                  "path": "/suggestions/short/index",
                  "name": "ShortSuggestion",
                  "meta": {
                    "title": "短建议列表",
                  }
                },
                {
                  "path": "/suggestions/long/index",
                  "name": "LongSuggestion",
                  "meta": {
                    "title": "长建议列表"
                  }
                },
              ]})";
      std::string noticesRouterJsonStr =
          R"({ "path": "/notices",
              "meta": {
                "title": "公告管理",
                "icon": "ep:bell",
              },
              "children": [
                {
                  "path": "/notices/list/index",
                  "name": "NoticeList",
                  "meta": {
                    "title": "公告列表",
                    "showParent": true
                  }
                }
              ]})";
      std::string teachersRouterJsonStr =
          R"({ "path": "/teachers",
              "meta": {
                "title": "教师管理",
                "icon": "ep:user",
              },
              "children": [
                {
                  "path": "/teachers/list/index",
                  "name": "TeacherList",
                  "meta": {
                    "title": "教师列表",
                    "showParent": true
                  }
                }
              ]})";
      std::string platformRouterJsonStr =
          R"({ "path": "/platform",
              "meta": {
                "title": "平台设置",
                "icon": "ep:setting",
              },
              "children": [
                {
                  "path": "/platform/settings/index",
                  "name": "PlatformSettings",
                  "meta": {
                    "title": "网站设置",
                  }
                },
                {
                  "path": "/platform/exam/index",
                  "name": "PlatformExam",
                  "meta": {
                    "title": "入站考试题库",
                  }
                }
              ]})";
};
