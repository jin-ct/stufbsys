#pragma once

#include <drogon/HttpFilter.h>

using namespace drogon;

class AdminLoginFilter : public HttpFilter<AdminLoginFilter> {
public:
    virtual void doFilter(const HttpRequestPtr& req, FilterCallback&& fcb, FilterChainCallback&& fccb) override;
};
