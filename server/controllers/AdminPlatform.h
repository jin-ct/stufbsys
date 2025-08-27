#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "third_party/libbcrypt/include/bcrypt/BCrypt.hpp"
#include "../models/EntranceExam.h"
#include "../models/AdminAccount.h"
#include "../models/PlatformSettings.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminPlatform : public drogon::HttpController<AdminPlatform>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(AdminPlatform::getQuestionList, "/exam/list", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::addQuestion, "/exam/add", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::updateQuestion, "/exam/update", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::deleteQuestion, "/exam/delete", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::setSettings, "/setting/set", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::getSettings, "/setting/get", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::setAdminAccountPassword, "/setting/admin/set/password", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::uploadLogo, "/setting/upload/logo", Post, "AdminLoginFilter");
      METHOD_ADD(AdminPlatform::uploadHomeImage, "/setting/upload/home/image", Post, "AdminLoginFilter");
    METHOD_LIST_END

    void getQuestionList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addQuestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, EntranceExam&& question) const;
    void updateQuestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, EntranceExam&& question) const;
    void deleteQuestion(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void setSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getSettings(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void setAdminAccountPassword(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void uploadLogo(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void uploadHomeImage(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;

};