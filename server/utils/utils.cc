#include "utils.h"
#include "../models/PlatformSettings.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::stufbsys;

std::string getExpiresFromCurrentTime(const int& sessionTime)
{
    auto now = std::chrono::system_clock::now();
    auto expires = now + std::chrono::seconds(sessionTime);
    std::time_t expires_c = std::chrono::system_clock::to_time_t(expires);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &expires_c);
#else
    localtime_r(&expires_c, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y/%m/%d %H:%M:%S");
    return oss.str();
}

drogon::HttpResponsePtr getJsonDataResponse(const bool& success, const Json::Value& data, const drogon::HttpStatusCode& statusCode)
{
    Json::Value ret;
    ret["success"] = success;
    ret["data"] = data;
    auto response = drogon::HttpResponse::newHttpJsonResponse(ret);
    response->setStatusCode(statusCode);
    return response;
}

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter))
    {
        result.push_back(item);
    }
    return result;
}

std::optional<Json::Value> parseJsonString(const std::string& jsonStr) noexcept
{
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(jsonStr);
    if (!Json::parseFromStream(builder, iss, &root, &errs)) {
        return std::nullopt;
    }
    return root;
}

std::string fromHex(const std::string& hex) {
    std::string result;
    result.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        unsigned int byte;
        std::istringstream(hex.substr(i, 2)) >> std::hex >> byte;
        result.push_back(static_cast<char>(byte));
    }
    return result;
}

std::string toHex(const std::string& binary) {
    std::ostringstream oss;
    for (unsigned char c : binary) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return oss.str();
}

std::string encryptText(const std::string& plainText)
{
    auto key = drogon::app().getCustomConfig()["encryptTextKey"].asString();
    auto iv = drogon::app().getCustomConfig()["encryptTextIv"].asString();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

    std::vector<unsigned char> out(plainText.size() + EVP_MAX_BLOCK_LENGTH);
    int outLen1 = 0, outLen2 = 0;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        reinterpret_cast<const unsigned char*>(iv.data()))) {
        LOG_ERROR << "EVP_EncryptInit_ex failed";
        return "";
    }
    if (1 != EVP_EncryptUpdate(ctx, out.data(), &outLen1,
        reinterpret_cast<const unsigned char*>(plainText.data()), plainText.size())) {
        LOG_ERROR << "EVP_EncryptUpdate failed";
        return "";
    }
    if (1 != EVP_EncryptFinal_ex(ctx, out.data() + outLen1, &outLen2)) {
        LOG_ERROR << "EVP_EncryptFinal_ex failed";
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);
    return toHex(std::string(reinterpret_cast<char*>(out.data()), outLen1 + outLen2));
}

std::string decryptText(const std::string& cipherHex)
{
    auto key = drogon::app().getCustomConfig()["encryptTextKey"].asString();
    auto iv = drogon::app().getCustomConfig()["encryptTextIv"].asString();

    auto cipherText = fromHex(cipherHex);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

    std::vector<unsigned char> out(cipherText.size());
    int outLen1 = 0, outLen2 = 0;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
        reinterpret_cast<const unsigned char*>(key.data()),
        reinterpret_cast<const unsigned char*>(iv.data()))) {
        LOG_ERROR << "EVP_DecryptInit_ex failed";
        return "";
    }

    if (1 != EVP_DecryptUpdate(ctx, out.data(), &outLen1,
        reinterpret_cast<const unsigned char*>(cipherText.data()), cipherText.size())) {
        LOG_ERROR << "EVP_DecryptUpdate failed";
        return "";
    }

    if (1 != EVP_DecryptFinal_ex(ctx, out.data() + outLen1, &outLen2)) {
        LOG_ERROR << "EVP_DecryptFinal_ex failed";
        return "";
    }

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(out.data()), outLen1 + outLen2);
}

std::string generateRandomNumber(const int& digits, const bool& withDecimal, int decimalDigits) {
    if (digits <= 0) return "0";
    if (withDecimal) {
        if (decimalDigits <= 0) decimalDigits = 1;
        if (decimalDigits >= digits) decimalDigits = digits - 1;
        int intPartLen = digits - decimalDigits;
        if (intPartLen <= 0) intPartLen = 1;
        // 生成整数部分
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> firstDigitDist(1, 9);
        std::uniform_int_distribution<> digitDist(0, 9);

        std::string intPart;
        intPart += std::to_string(firstDigitDist(gen));
        for (int i = 1; i < intPartLen; ++i) {
            intPart += std::to_string(digitDist(gen));
        }
        // 生成小数部分
        std::string fracPart;
        for (int i = 0; i < decimalDigits; ++i) {
            fracPart += std::to_string(digitDist(gen));
        }
        return intPart + "." + fracPart;
    }
    else {
        // 纯整数
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> firstDigitDist(1, 9);
        std::uniform_int_distribution<> digitDist(0, 9);

        std::string result;
        result += std::to_string(firstDigitDist(gen));
        for (int i = 1; i < digits; ++i) {
            result += std::to_string(digitDist(gen));
        }
        return result;
    }
}

std::string getRandomString(const size_t& n)
{
    std::vector<unsigned char> random(n);
    utils::secureRandomBytes(random.data(), random.size());

    // This is cryptographically safe as 256 mod 16 == 0
    static const std::string alphabets = "0123456789abcdef";
    assert(256 % alphabets.size() == 0);
    std::string randomString(n, '\0');
    for (size_t i = 0; i < n; i++)
        randomString[i] = alphabets[random[i] % alphabets.size()];
    return randomString;
}

std::pair<std::string, std::string> parseHostAndPath(const std::string& url)
{
    static const std::regex urlRegex(R"(^(https?://[^/]+)(/.*)?$)", std::regex::icase);
    std::smatch match;
    if (std::regex_match(url, match, urlRegex)) {
        std::string host = match[1].str();
        std::string path = match[2].matched ? match[2].str() : "/";
        return { host, path };
    }
    return { "", "" };
}

void requestAiApi(const Json::Value& messages, const std::function<void(Json::Value)> &callback)
{
    LOG_DEBUG << "Request Ai Api";
    auto dbClientPtr = drogon::app().getDbClient();
    Mapper<PlatformSettings> mp(dbClientPtr);
    mp.findBy(
        Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "ai_api_link") ||
        Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "ai_api_key") ||
        Criteria(PlatformSettings::Cols::_name, CompareOperator::EQ, "ai_model_name"),
        [callback, messages](const std::vector<PlatformSettings>& values) {
            std::string link = "";
            std::string key = "";
            std::string modelName = "";
            for (const auto& value : values) {
                if (value.getValueOfName() == "ai_api_link") link = value.getValueOfValue();
                if (value.getValueOfName() == "ai_model_name") modelName = value.getValueOfValue();
                if (value.getValueOfName() == "ai_api_key") key = decryptText(value.getValueOfValue());
            }
            Json::Value requestData{};
            requestData["model"] = modelName;
            requestData["messages"] = messages;
            Json::Value format{};
            format["type"] = "json_object";
            requestData["response_format"] = format;

            auto [host, path] = parseHostAndPath(link);

            auto client = drogon::HttpClient::newHttpClient(host);
            auto req = drogon::HttpRequest::newHttpJsonRequest(requestData);
            req->setMethod(Post);
            req->addHeader("Authorization", "Bearer " + key);
            req->setPath(path);
            client->sendRequest(
                req,
                [callback](drogon::ReqResult result, const drogon::HttpResponsePtr& resp) {
                    if (result == drogon::ReqResult::Ok && resp->getStatusCode() == drogon::k200OK) {
                        auto json = resp->getJsonObject();
                        if (json->isMember("choices")) {
                            Json::Value res{};
                            res["success"] = true;
                            res["data"] = *json;
                            callback(res);
                        }
                        else {
                            LOG_ERROR << "Ai Result Data error";
                        }
                    }
                    else
                    {
                        Json::Value json{};
                        if (result == drogon::ReqResult::Ok) {
                            if (resp->getContentType() == drogon::CT_APPLICATION_JSON) json = *(resp->getJsonObject());
                            LOG_DEBUG << "getStatusCode(): " << resp->getStatusCode();
                        }
                        LOG_ERROR << "Ai Request error: " << (json.isMember("message") ? json["message"].asString() : "UnknownError");
                        Json::Value res{};
                        res["success"] = false;
                        callback(res);
                    }
                },
                30.0
            );
        },
        [callback](const DrogonDbException& e) {
            LOG_ERROR << "Database error: " << e.base().what();
            Json::Value res{};
			res["success"] = false;
            callback(res);
        }
    );
}

void execSqlAsyncNoCallback(const std::string& sql, const std::string& successLog, const std::string& errorLog)
{
    auto dbClientPtr = drogon::app().getDbClient();
    dbClientPtr->execSqlAsync(
        sql,
        [successLog](const Result& r) {
            LOG_DEBUG << successLog;
        },
        [errorLog](const DrogonDbException& e) {
            LOG_ERROR << errorLog << ", Error=" << e.base().what();
        }
    );
}
