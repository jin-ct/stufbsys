#include <drogon/drogon.h>
#include "UserLoginFilter.h"
#include "../plugins/JwtPlugin.h"
#include <iostream>

using namespace drogon;

void UserLoginFilter::doFilter(const HttpRequestPtr& req, FilterCallback&& fcb, FilterChainCallback&& fccb) {
    if (req->getHeader("Authorization").empty()) {
        LOG_ERROR << "Missing Authorization header";
        Json::Value ret;
        ret["error"] = "missing Authorization header";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k400BadRequest);
        fcb(resp);
        return;
    }

    auto token = req->getHeader("Authorization").substr(7);
    auto* jwtPtr = drogon::app().getPlugin<JwtPlugin>();
    auto jwt = jwtPtr->init();
    auto decoded = jwt.decode(token);
    if (decoded.has_value() && !decoded.value().get_payload_json()["user_id"].to_str().empty()) {
        auto userId = std::stoll(decoded.value().get_payload_json()["user_id"].to_str());
        req->attributes()->insert("userId", userId);
        fccb();
    }
    else {
        LOG_ERROR << "Invalid token";
        Json::Value ret;
        ret["error"] = "invalid token";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
    }
}