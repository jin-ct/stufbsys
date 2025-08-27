#pragma once

#include <drogon/HttpController.h>
#include "third_party/libbcrypt/include/bcrypt/BCrypt.hpp"
#include "../models/Teacher.h"
#include "../models/PlatformSettings.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class TeacherSettings : public drogon::HttpController<TeacherSettings>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(TeacherSettings::setSettings, "/set", Post, "TeacherLoginFilter");
      METHOD_ADD(TeacherSettings::getSettings, "/get-settings", Post, "TeacherLoginFilter");
      METHOD_ADD(TeacherSettings::getBottomSaying, "/saying/get", Get, "TeacherLoginFilter");
      METHOD_ADD(TeacherSettings::adminResetVerify, "/admin-reset-verify", Post, "TeacherLoginFilter");
    METHOD_LIST_END

    void setSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getBottomSaying(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void adminResetVerify(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
