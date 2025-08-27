#include <drogon/drogon.h>
#include "AdminLoginFilter.h"
#include "../plugins/JwtPlugin.h"

using namespace drogon;

void AdminLoginFilter::doFilter(const HttpRequestPtr& req, FilterCallback&& fcb, FilterChainCallback&& fccb) {
    if (req->getHeader("Authorization").empty()) {
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
    if (decoded.has_value() && !decoded.value().get_payload_json()["userId"].to_str().empty()) {
         fccb();
    }
    else {
        Json::Value ret;
        ret["error"] = "invalid token";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
    }
}