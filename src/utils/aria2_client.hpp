#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace tglink2aria2 {
namespace utils {

class Aria2Client {
 public:
  Aria2Client(const std::string& url, int port, const std::string& token);
  ~Aria2Client() = default;

  // Add a download task
  std::string addUri(const std::string& uri, const std::string& filename);

  // Get download status
  nlohmann::json getStatus(const std::string& gid);

  // Remove a download task
  bool remove(const std::string& gid);

  // Pause a download task
  bool pause(const std::string& gid);

  // Resume a paused download task
  bool resume(const std::string& gid);

  // Get global statistics
  nlohmann::json getGlobalStat();

  // Get active downloads
  std::vector<nlohmann::json> getActiveDownloads();

  // Get waiting downloads
  std::vector<nlohmann::json> getWaitingDownloads();

  // Get stopped downloads
  std::vector<nlohmann::json> getStoppedDownloads();

  // Get completed downloads
  std::vector<nlohmann::json> getCompletedDownloads();

 private:
  nlohmann::json callRPC(
      const std::string& method,
      const nlohmann::json& params = nlohmann::json::array());

  std::string url_;
  int port_;
  std::string token_;
  std::string secret_;
};

}  // namespace utils
}  // namespace tglink2aria2