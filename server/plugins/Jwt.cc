#include <drogon/drogon.h>
#include <utility>
#include "Jwt.h"

Jwt::Jwt(const std::string &secret, const int sessionTime, const std::string &issuer) :
  secret{std::move(secret)}, sessionTime{sessionTime}, issuer{std::move(issuer)} {}

auto Jwt::encode(const std::string &field, const int64_t value) -> std::string {
    auto time = std::chrono::system_clock::now();
    auto expiresAt = std::chrono::duration_cast<std::chrono::seconds>((time + std::chrono::seconds{sessionTime}).time_since_epoch()).count();
    auto token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWS")
        .set_issued_at(time)
        .set_expires_at(std::chrono::system_clock::from_time_t(expiresAt))
        .set_payload_claim(field, jwt::claim(std::to_string(value)))
        .sign(jwt::algorithm::hs256{secret});
    return token;
}

auto Jwt::encode(const int64_t userId, const int role) -> std::string
{
    auto time = std::chrono::system_clock::now();
    auto expiresAt = std::chrono::duration_cast<std::chrono::seconds>((time + std::chrono::seconds{ sessionTime }).time_since_epoch()).count();
    auto token = jwt::create()
        .set_issuer(issuer)
        .set_type("JWS")
        .set_issued_at(time)
        .set_expires_at(std::chrono::system_clock::from_time_t(expiresAt))
        .set_payload_claim("userId", jwt::claim(std::to_string(userId)))
        .set_payload_claim("role", jwt::claim(std::to_string(role)))
        .sign(jwt::algorithm::hs256{ secret });
    return token;
}

std::optional<jwt::decoded_jwt<jwt::traits::kazuho_picojson>>
Jwt::decode(const std::string& token) noexcept {
    auto verifier = jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{secret})
        .with_issuer(issuer);
    try {
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
        return decoded;
    }
    catch (const std::invalid_argument& e) {
        LOG_ERROR << "Token decode error:" << e.what();
        return std::nullopt;
    }
    catch (const std::runtime_error& e) {
        LOG_ERROR << "Token decode error: " << e.what();
        return std::nullopt;
    }
    catch (const std::exception& e) {
        LOG_ERROR << "Unexpected error: " << e.what();
        return std::nullopt;
    }
}
