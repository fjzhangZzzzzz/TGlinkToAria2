#pragma once

#include <memory>
#include <string>
#include <vector>

#include "tgbot/tgbot.h"
#include "utils/aria2_client.hpp"

namespace tglink2aria2 {
namespace bot {

class Bot {
 public:
  Bot(const std::string& token, const std::string& api_id,
      const std::string& api_hash, const std::string& bin_channel,
      const std::string& proxy = "", int port = 8080,
      const std::string& bind_address = "0.0.0.0");
  ~Bot() = default;

  // Start the bot
  void start();

  // Stop the bot
  void stop();

 private:
  // Handle incoming messages
  void handleMessage(TgBot::Message::Ptr message);

  // Handle file messages
  void handleFile(TgBot::Message::Ptr message);

  // Generate stream link for a file
  std::string generateStreamLink(const std::string& file_id,
                                 const std::string& filename);

  // Add file to Aria2
  void addToAria2(const std::string& file_url, const std::string& filename);

  // Send status message
  void sendStatusMessage(int64_t chat_id, const std::string& gid);

  // Check download status
  void checkDownloadStatus(const std::string& gid);

  std::unique_ptr<TgBot::Bot> bot_;
  std::unique_ptr<utils::Aria2Client> aria2_client_;
  std::string bin_channel_;
  int port_;
  std::string bind_address_;
  bool running_;
};

}  // namespace bot
}  // namespace tglink2aria2