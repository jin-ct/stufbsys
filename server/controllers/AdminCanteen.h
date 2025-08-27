#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/Dish.h"
#include "../models/DishAdd.h"
#include "../models/DishEvaluate.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class AdminCanteen : public drogon::HttpController<AdminCanteen>
{
  public:
	METHOD_LIST_BEGIN
	  METHOD_ADD(AdminCanteen::getDishList, "/dish/list", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::addDish, "/dish/add", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::updateDish, "/dish/update", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::deleteDish, "/dish/delete", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::importDishes, "/dish/import", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::getAllEvaluate, "/evaluate/all", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::getEvaluateList, "/evaluate/list", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::deleteEvaluate, "/evaluate/delete", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::getDishAddList, "/dish/add/list", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::deleteDishAdd, "/dish/add/delete", Post, "AdminLoginFilter");
	  METHOD_ADD(AdminCanteen::approveDishAdd, "/dish/add/approve", Post, "AdminLoginFilter");
    METHOD_LIST_END

	void getDishList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void addDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Dish&& dish) const;
	void updateDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, Dish&& dish) const;
	void deleteDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void importDishes(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void getAllEvaluate(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void getEvaluateList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void deleteEvaluate(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void getDishAddList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void deleteDishAdd(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
	void approveDishAdd(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
private:
};
