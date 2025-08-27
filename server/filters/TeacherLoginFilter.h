#pragma once

#include <drogon/HttpFilter.h>

using namespace drogon;

class TeacherLoginFilter : public HttpFilter<TeacherLoginFilter> {
public:
    virtual void doFilter(const HttpRequestPtr& req, FilterCallback&& fcb, FilterChainCallback&& fccb) override;
};
