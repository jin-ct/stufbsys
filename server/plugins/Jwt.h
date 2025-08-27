#pragma once

#include <third_party/jwt-cpp/include/jwt-cpp/jwt.h>
#include <string>
#include <optional>

class Jwt {
 public:
    Jwt(const std::string &secret, const int sessionTime, const std::string &issuer);
    auto encode(const std::string &field, const int64_t value) -> std::string;
    auto encode(const int64_t userId, const int role) -> std::string;
    std::optional<jwt::decoded_jwt<jwt::traits::kazuho_picojson>> decode(const std::string& token) noexcept;

 private:
    std::string secret;
    int sessionTime;
    std::string issuer;
};
