#include "bot.hpp"

#include <spdlog/spdlog.h>

#include "vars/vars.hpp"

namespace tglink2aria2 {
namespace bot {

Bot::Bot(const std::string& token, const std::string& api_id,
         const std::string& api_hash, const std::string& bin_channel,
         const std::string& proxy, int port, const std::string& bind_address)
    : bot_(std::make_unique<TgBot::Bot>(token)),
      bin_channel_(bin_channel),
      port_(port),
      bind_address_(bind_address),
      running_(false) {
  // Initialize Aria2 client if enabled
  if (vars::Vars::getInstance().isAria2Enabled()) {
    const auto& rpc_urls = vars::Vars::getInstance().getRpcUrls();
    const auto& rpc_ports = vars::Vars::getInstance().getRpcPorts();
    const auto& rpc_tokens = vars::Vars::getInstance().getRpcTokens();

    if (!rpc_urls.empty() && !rpc_ports.empty() && !rpc_tokens.empty()) {
      aria2_client_ = std::make_unique<utils::Aria2Client>(
          rpc_urls[0], std::stoi(rpc_ports[0]), rpc_tokens[0]);
      spdlog::info("Aria2 client initialized with URL: {}, Port: {}, Token: {}",
                   rpc_urls[0], rpc_ports[0], rpc_tokens[0]);
    } else {
      spdlog::warn("Aria2 is enabled but RPC configuration is incomplete");
    }
  }

  // Set up message handlers
  bot_->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
    bot_->getApi().sendMessage(message->chat->id,
                               "Welcome to TGlinkToAria2 bot!");
  });

  bot_->getEvents().onAnyMessage(
      [this](TgBot::Message::Ptr message) { handleMessage(message); });
}

void Bot::start() {
  spdlog::info("Starting bot...");
  running_ = true;

  try {
    bot_->getApi().deleteWebhook();
    bot_->getApi().setWebhook("https://your-domain.com/bot" + bot_->getToken());
  } catch (const std::exception& e) {
    spdlog::error("Failed to set webhook: {}", e.what());
    return;
  }

  spdlog::info("Bot started successfully");
}

void Bot::stop() {
  spdlog::info("Stopping bot...");
  running_ = false;
}

void Bot::handleMessage(TgBot::Message::Ptr message) {
  if (message->document) {
    handleFile(message);
  } else if (message->video) {
    handleFile(message);
  } else if (message->audio) {
    handleFile(message);
  } else if (message->voice) {
    handleFile(message);
  } else if (message->videoNote) {
    handleFile(message);
  } else if (message->photo.size() > 0) {
    handleFile(message);
  } else if (message->sticker) {
    handleFile(message);
  }
}

void Bot::handleFile(TgBot::Message::Ptr message) {
  try {
    // Get file information
    std::string file_id;
    std::string filename;

    if (message->document) {
      file_id = message->document->fileId;
      filename = message->document->fileName;
    } else if (message->video) {
      file_id = message->video->fileId;
      filename = "video.mp4";
    } else if (message->audio) {
      file_id = message->audio->fileId;
      filename = message->audio->fileName;
    } else if (message->voice) {
      file_id = message->voice->fileId;
      filename = "voice.ogg";
    } else if (message->videoNote) {
      file_id = message->videoNote->fileId;
      filename = "video_note.mp4";
    } else if (message->photo.size() > 0) {
      file_id = message->photo.back()->fileId;
      filename = "photo.jpg";
    } else if (message->sticker) {
      file_id = message->sticker->fileId;
      filename = "sticker.webp";
    }

    // Generate stream link
    std::string stream_link = generateStreamLink(file_id, filename);

    // Send stream link to user
    std::string response = "Stream link: " + stream_link + "\n\n";

    // Only show Aria2 button if Aria2 is enabled and configured
    if (vars::Vars::getInstance().isAria2Enabled() && aria2_client_) {
      response += "Would you like to add this to Aria2?";

      auto keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
      std::vector<TgBot::InlineKeyboardButton::Ptr> row;
      auto button = std::make_shared<TgBot::InlineKeyboardButton>();
      button->text = "Add to Aria2";
      button->callbackData = "add_to_aria2:" + file_id + ":" + filename;
      row.push_back(button);
      keyboard->inlineKeyboard.push_back(row);

      bot_->getApi().sendMessage(message->chat->id, response, false, 0,
                                 keyboard);
    } else {
      bot_->getApi().sendMessage(message->chat->id, response);
    }

  } catch (const std::exception& e) {
    spdlog::error("Failed to handle file: {}", e.what());
    bot_->getApi().sendMessage(message->chat->id,
                               "Sorry, something went wrong.");
  }
}

std::string Bot::generateStreamLink(const std::string& file_id,
                                    const std::string& filename) {
  // Get file path from Telegram
  auto file = bot_->getApi().getFile(file_id);
  std::string file_path = file->filePath;

  // Generate stream link
  return "https://your-domain.com/stream/" + file_path;
}

void Bot::addToAria2(const std::string& file_url, const std::string& filename) {
  try {
    // Add to Aria2
    std::string gid = aria2_client_->addUri(file_url, filename);
    spdlog::info("Added file to Aria2 with GID: {}", gid);

    // Start checking download status
    checkDownloadStatus(gid);
  } catch (const std::exception& e) {
    spdlog::error("Failed to add file to Aria2: {}", e.what());
    throw;
  }
}

void Bot::sendStatusMessage(int64_t chat_id, const std::string& gid) {
  try {
    auto status = aria2_client_->getStatus(gid);
    std::string status_message = "Download Status:\n";
    status_message +=
        "Filename: " + status["files"][0]["path"].get<std::string>() + "\n";
    status_message +=
        "Size: " + std::to_string(status["totalLength"].get<int64_t>()) +
        " bytes\n";
    status_message += "Downloaded: " +
                      std::to_string(status["completedLength"].get<int64_t>()) +
                      " bytes\n";
    status_message +=
        "Speed: " + std::to_string(status["downloadSpeed"].get<int64_t>()) +
        " B/s\n";
    status_message +=
        "Progress: " + std::to_string(status["progress"].get<double>() * 100) +
        "%";

    bot_->getApi().sendMessage(chat_id, status_message);
  } catch (const std::exception& e) {
    spdlog::error("Failed to get download status: {}", e.what());
  }
}

void Bot::checkDownloadStatus(const std::string& gid) {
  // This would typically be implemented using a timer or event system
  // to periodically check the download status and send updates
  // For simplicity, we're just showing a basic implementation
  try {
    auto status = aria2_client_->getStatus(gid);
    if (status["status"] == "complete") {
      spdlog::info("Download completed: {}", gid);
    } else if (status["status"] == "error") {
      spdlog::error("Download failed: {}", gid);
    }
  } catch (const std::exception& e) {
    spdlog::error("Failed to check download status: {}", e.what());
  }
}

}  // namespace bot
}  // namespace tglink2aria2