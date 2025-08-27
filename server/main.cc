#include <drogon/drogon.h>
#include "../utils/EverydayTasks.h"
#include <iostream>

int main() {
    try {
        drogon::app().loadConfigFile(std::filesystem::absolute("config.json").string());
    }
    catch
		(const std::exception& e) {
		std::cerr << "Error loading configuration: " << e.what() << std::endl;
		return 1;
	}
    drogon::app().setExceptionHandler(
        [](const std::exception& ex,
           const drogon::HttpRequestPtr& req,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
                LOG_ERROR << "Exception: " << ex.what();
                callback(getJsonDataResponse(false, Json::Value(), drogon::k500InternalServerError));
        }
    );
    startEverydayTasks();
    drogon::app().run();
    return 0;
}
