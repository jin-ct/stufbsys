#pragma once

#include <drogon/drogon.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <vector>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <regex>
#include <random>
#include <optional>

std::string getExpiresFromCurrentTime(const int& sessionTime);
drogon::HttpResponsePtr getJsonDataResponse(const bool& success,
	                                        const Json::Value& data = Json::Value(),
	                                        const drogon::HttpStatusCode& statusCode = drogon::k200OK);
std::vector<std::string> split(const std::string& str, char delimiter);
std::optional<Json::Value> parseJsonString(const std::string& jsonStr) noexcept;
std::string toHex(const std::string& binary);
std::string fromHex(const std::string& hex);
std::string encryptText(const std::string& plainText);
std::string decryptText(const std::string& cipherText);
std::string generateRandomNumber(const int& digits, const bool& withDecimal = false, int decimalDigits = 2);
std::string getRandomString(const size_t& n);
std::pair<std::string, std::string> parseHostAndPath(const std::string& url);
void requestAiApi(const Json::Value& messages, const std::function<void(Json::Value)> &callback);
void execSqlAsyncNoCallback(const std::string& sql, const std::string& successLog, const std::string& errorLog);

template<typename T = std::string>
void dbDataIncrementOne(const std::string& tableName,
	                    const std::string& columnName,
                        const bool& isDataStatisticsToday = true,
	                    const std::string& findByColumnName = "",
	                    const T& findByValue = T())
{
    std::string sql = "UPDATE \"" + tableName + "\" SET \"" + columnName + "\" = \"" + columnName + "\" + 1";
    std::string findByValueStr = "";
    if constexpr (std::is_arithmetic_v<T>) {
        findByValueStr = std::to_string(findByValue);
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        findByValueStr = "'" + findByValue + "'";
    }

    if (isDataStatisticsToday && !findByColumnName.empty()) {
        sql += " WHERE \"date\" = CURRENT_DATE";
        sql += " AND \"" + findByColumnName + "\" = " + findByValueStr;
    }
    else if (isDataStatisticsToday)
    {
        sql += " WHERE \"date\" = CURRENT_DATE";
    }
    else if (!findByColumnName.empty())
    {
        sql += " WHERE \"" + findByColumnName + "\" = " + findByValueStr;
    }
	execSqlAsyncNoCallback(
        sql,
        "Database Data Increment One success: Table=" + tableName + ", Column=" + columnName,
        "Database Data Increment One error: Table=" + tableName + ", Column=" + columnName
    );
}
