#pragma once

#include <drogon/HttpFilter.h>

using namespace drogon;

class UserLoginFilter : public HttpFilter<UserLoginFilter> {
public:
    virtual void doFilter(const HttpRequestPtr& req, FilterCallback&& fcb, FilterChainCallback&& fccb) override;
};
