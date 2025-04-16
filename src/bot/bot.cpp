#include "bot.hpp"

#include <spdlog/spdlog.h>

#include "server/server.hpp"
#include "utils/file_properties.hpp"

namespace tglink2aria2 {
namespace bot {

Bot::Bot() {
  // Initialize bot
  bot_ = std::make_unique<TgBot::Bot>(vars::Vars::getInstance().getBotToken());

  // Initialize server
  server_ = std::make_unique<server::Server>(
      vars::Vars::getInstance().getBindAddress(),
      vars::Vars::getInstance().getPort(), *bot_);

  // Setup handlers
  setupHandlers();
}

Bot::~Bot() { stop(); }

void Bot::start() {
  try {
    spdlog::info("Starting bot...");
    bot_->getApi().deleteWebhook();
    server_->start();
  } catch (const std::exception& e) {
    spdlog::error("Failed to start bot: {}", e.what());
    throw;
  }
}

void Bot::stop() {
  if (server_) {
    server_->stop();
  }
}

void Bot::setupHandlers() {
  // Start command
  bot_->getEvents().onCommand(
      "start",
      [this](const TgBot::Message::Ptr& message) { handleStart(message); });

  // Media messages
  bot_->getEvents().onAnyMessage([this](const TgBot::Message::Ptr& message) {
    if (message->document || message->video || message->audio ||
        message->voice || message->videoNote || !message->photo.empty() ||
        message->sticker) {
      handleMedia(message);
    }
  });
}

void Bot::handleStart(const TgBot::Message::Ptr& message) {
  try {
    std::string response = "Hi " + message->from->firstName +
                           "! Send me a file to get an instant stream link.";
    bot_->getApi().sendMessage(message->chat->id, response);
  } catch (const std::exception& e) {
    spdlog::error("Error handling start command: {}", e.what());
  }
}

void Bot::handleMedia(const TgBot::Message::Ptr& message) {
  try {
    // Forward message to bin channel
    auto forwarded =
        bot_->getApi().forwardMessage(vars::Vars::getInstance().getBinChannel(),
                                      message->chat->id, message->messageId);

    // Parse file properties
    utils::FileProperties file_props(forwarded);

    // Generate hash
    std::string hash = server_->generateHash(file_props.getFileUniqueId(), 6);

    // Generate links
    std::string long_link =
        "http://" + vars::Vars::getInstance().getBindAddress() + ":" +
        std::to_string(vars::Vars::getInstance().getPort()) + "/" +
        std::to_string(forwarded->messageId) + "?hash=" + hash;
    std::string short_link =
        "http://" + vars::Vars::getInstance().getBindAddress() + ":" +
        std::to_string(vars::Vars::getInstance().getPort()) + "/" + hash +
        std::to_string(forwarded->messageId);

    // Send response
    std::string response =
        "Long link: " + long_link + "\nShort link: " + short_link;
    bot_->getApi().sendMessage(message->chat->id, response);

    // Add to Aria2 if enabled
    if (vars::Vars::getInstance().isAria2Enabled()) {
      // TODO: Implement Aria2 integration
    }

  } catch (const std::exception& e) {
    spdlog::error("Error handling media message: {}", e.what());
    bot_->getApi().sendMessage(
        message->chat->id, "Error processing file: " + std::string(e.what()));
  }
}

}  // namespace bot
}  // namespace tglink2aria2