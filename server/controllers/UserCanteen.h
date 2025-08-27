#pragma once

#include <drogon/HttpController.h>
#include <iostream>
#include "../models/Dish.h"
#include "../models/DishAdd.h"
#include "../models/DishEvaluate.h"
#include "../models/VoteRecord.h"
#include "../utils/utils.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

class UserCanteen : public drogon::HttpController<UserCanteen>
{
  public:
    METHOD_LIST_BEGIN
      METHOD_ADD(UserCanteen::getDishList, "/dish/list", Get);
      METHOD_ADD(UserCanteen::evaluateDish, "/dish/evaluate", Post, "UserLoginFilter");
      METHOD_ADD(UserCanteen::getEvaluateList, "/evaluate/list", Get);
      METHOD_ADD(UserCanteen::getDishAddList, "/dish-add/list", Get);
      METHOD_ADD(UserCanteen::addDishAdd, "/dish-add/add", Post, "UserLoginFilter");
      METHOD_ADD(UserCanteen::addVote, "/vote", Get, "UserLoginFilter");
    METHOD_LIST_END

    void getDishList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void evaluateDish(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getEvaluateList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void getDishAddList(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addDishAdd(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
    void addVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;

    static void updateDishAddAiScore(const DishAdd& dish, const int& tryCount = 0);
};
