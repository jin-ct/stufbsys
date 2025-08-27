#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "third_party/libbcrypt/include/bcrypt/BCrypt.hpp"
#include "../models/Teacher.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminTeacher : public drogon::HttpController<AdminTeacher>
{
  public:
    METHOD_LIST_BEGIN
	  METHOD_ADD(AdminTeacher::getTeacherList, "/list", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminTeacher::addTeacher, "/add", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminTeacher::updateTeacher, "/update", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminTeacher::deleteTeacher, "/delete", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminTeacher::importTeachers, "/import", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminTeacher::resetPassword, "/reset", Post, "AdminLoginFilter");
    METHOD_LIST_END

	void getTeacherList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void addTeacher(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Teacher&& teacher) const;
	void updateTeacher(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Teacher&& teacher) const;
	void deleteTeacher(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void importTeachers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void resetPassword(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
};
