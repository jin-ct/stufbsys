#pragma once

#include <drogon/HttpController.h>
#include "../models/Teacher.h"
#include "../models/PlatformSettings.h"
#include "third_party/libbcrypt/include/bcrypt/BCrypt.hpp"
#include "../plugins/JwtPlugin.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class TeacherLogin : public drogon::HttpController<TeacherLogin>
{
  public:
      METHOD_LIST_BEGIN
        METHOD_ADD(TeacherLogin::login, "/login", Post);
        METHOD_ADD(TeacherLogin::refreshTokenApi, "/refresh-token", Post);
        METHOD_ADD(TeacherLogin::getAsyncRoutes, "/get-async-routes", Get);
      METHOD_LIST_END

      void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Teacher&& pNewUser) const;
      void refreshTokenApi(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
      void getAsyncRoutes(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;

      static std::string getToken(const Teacher& user);
      static drogon::HttpResponsePtr getTchLoginJsonDataResponse(const Teacher& tch, std::shared_ptr<Teacher> pNewUserPtr);

  private:
      std::string suggestionsRouterJsonStr =
          R"({ "path": "/suggestions",
                "meta": {
                  "title": "建议管理",
                  "icon": "ep:chat-dot-square"
                },
                "children": [
                {
                  "path": "/suggestions/index",
                  "name": "Suggestion",
                  "meta": {
                     "title": "建议列表",
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
                  "path": "/platform/account/index",
                  "name": "accountSettings",
                  "meta": {
                    "title": "账号设置",
                    "showParent": true
                  }
                }
              ]})";
};
