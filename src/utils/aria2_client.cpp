#include "aria2_client.hpp"

#include <curl/curl.h>
#include <spdlog/spdlog.h>

#include <sstream>

namespace tglink2aria2 {
namespace utils {

Aria2Client::Aria2Client(const std::string& url, int port,
                         const std::string& token)
    : url_(url), port_(port), token_(token), secret_("token:" + token) {}

std::string Aria2Client::addUri(const std::string& uri,
                                const std::string& filename) {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(nlohmann::json::array({uri}));
    params.push_back(nlohmann::json::object({{"out", filename}}));

    auto response = callRPC("aria2.addUri", params);
    return response.get<std::string>();
  } catch (const std::exception& e) {
    spdlog::error("Failed to add URI to Aria2: {}", e.what());
    throw;
  }
}

nlohmann::json Aria2Client::getStatus(const std::string& gid) {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(gid);

    return callRPC("aria2.tellStatus", params);
  } catch (const std::exception& e) {
    spdlog::error("Failed to get status from Aria2: {}", e.what());
    throw;
  }
}

bool Aria2Client::remove(const std::string& gid) {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(gid);

    auto response = callRPC("aria2.remove", params);
    return response.get<std::string>() == gid;
  } catch (const std::exception& e) {
    spdlog::error("Failed to remove download from Aria2: {}", e.what());
    throw;
  }
}

bool Aria2Client::pause(const std::string& gid) {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(gid);

    auto response = callRPC("aria2.pause", params);
    return response.get<std::string>() == gid;
  } catch (const std::exception& e) {
    spdlog::error("Failed to pause download in Aria2: {}", e.what());
    throw;
  }
}

bool Aria2Client::resume(const std::string& gid) {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(gid);

    auto response = callRPC("aria2.unpause", params);
    return response.get<std::string>() == gid;
  } catch (const std::exception& e) {
    spdlog::error("Failed to resume download in Aria2: {}", e.what());
    throw;
  }
}

nlohmann::json Aria2Client::getGlobalStat() {
  try {
    return callRPC("aria2.getGlobalStat");
  } catch (const std::exception& e) {
    spdlog::error("Failed to get global statistics from Aria2: {}", e.what());
    throw;
  }
}

std::vector<nlohmann::json> Aria2Client::getActiveDownloads() {
  try {
    auto response = callRPC("aria2.tellActive");
    return response.get<std::vector<nlohmann::json>>();
  } catch (const std::exception& e) {
    spdlog::error("Failed to get active downloads from Aria2: {}", e.what());
    throw;
  }
}

std::vector<nlohmann::json> Aria2Client::getWaitingDownloads() {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(0);     // offset
    params.push_back(1000);  // num

    auto response = callRPC("aria2.tellWaiting", params);
    return response.get<std::vector<nlohmann::json>>();
  } catch (const std::exception& e) {
    spdlog::error("Failed to get waiting downloads from Aria2: {}", e.what());
    throw;
  }
}

std::vector<nlohmann::json> Aria2Client::getStoppedDownloads() {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(0);     // offset
    params.push_back(1000);  // num

    auto response = callRPC("aria2.tellStopped", params);
    return response.get<std::vector<nlohmann::json>>();
  } catch (const std::exception& e) {
    spdlog::error("Failed to get stopped downloads from Aria2: {}", e.what());
    throw;
  }
}

std::vector<nlohmann::json> Aria2Client::getCompletedDownloads() {
  try {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(0);     // offset
    params.push_back(1000);  // num

    auto response = callRPC("aria2.tellStopped", params);
    std::vector<nlohmann::json> all_stopped =
        response.get<std::vector<nlohmann::json>>();
    std::vector<nlohmann::json> completed;

    for (const auto& download : all_stopped) {
      if (download["status"] == "complete") {
        completed.push_back(download);
      }
    }

    return completed;
  } catch (const std::exception& e) {
    spdlog::error("Failed to get completed downloads from Aria2: {}", e.what());
    throw;
  }
}

nlohmann::json Aria2Client::callRPC(const std::string& method,
                                    const nlohmann::json& params) {
  CURL* curl = curl_easy_init();
  if (!curl) {
    throw std::runtime_error("Failed to initialize CURL");
  }

  // Prepare request
  nlohmann::json request = {
      {"jsonrpc", "2.0"}, {"id", "1"}, {"method", method}, {"params", params}};
  request["params"].insert(request["params"].begin(), secret_);

  std::string request_str = request.dump();
  std::string response_str;

  // Set up CURL options
  curl_easy_setopt(curl, CURLOPT_URL,
                   (url_ + ":" + std::to_string(port_)).c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request_str.length());
  curl_easy_setopt(
      curl, CURLOPT_WRITEFUNCTION,
      [](void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
      });
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_str);

  // Set headers
  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Perform request
  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    throw std::runtime_error("CURL request failed: " +
                             std::string(curl_easy_strerror(res)));
  }

  // Parse response
  auto response = nlohmann::json::parse(response_str);
  if (response.contains("error")) {
    throw std::runtime_error("Aria2 RPC error: " +
                             response["error"]["message"].get<std::string>());
  }

  return response["result"];
}

}  // namespace utils
}  // namespace tglink2aria2