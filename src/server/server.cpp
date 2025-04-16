#include "server.hpp"

#include <openssl/sha.h>
#include <spdlog/spdlog.h>

#include <iomanip>
#include <sstream>

namespace tglink2aria2 {
namespace server {

Server::Server(const std::string& bind_address, int port, TgBot::Bot& bot)
    : bind_address_(bind_address),
      port_(port),
      bot_(bot),
      server_(std::make_unique<httplib::Server>()),
      byte_streamer_(std::make_unique<utils::ByteStreamer>(bot)) {
  setupRoutes();
}

Server::~Server() { stop(); }

void Server::start() {
  spdlog::info("Starting server on {}:{}", bind_address_, port_);
  server_->listen(bind_address_, port_);
}

void Server::stop() {
  if (server_) {
    server_->stop();
  }
}

void Server::setupRoutes() {
  server_->Get("/", [](const httplib::Request&, httplib::Response& res) {
    res.set_content("Server is running", "text/plain");
  });

  server_->Get(R"(/(\d+))",
               [this](const httplib::Request& req, httplib::Response& res) {
                 handleStream(req, res);
               });
}

void Server::handleStream(const httplib::Request& req, httplib::Response& res) {
  try {
    // Parse message ID from path
    int64_t message_id = std::stoll(req.matches[1]);

    // Get message from Telegram
    auto message = bot_.getApi().getMessage(
        vars::Vars::getInstance().getBinChannel(), message_id);
    if (!message) {
      res.status = 404;
      res.set_content("Message not found", "text/plain");
      return;
    }

    // Parse file properties
    utils::FileProperties file_props(message);

    // Check hash if provided
    if (req.has_param("hash")) {
      std::string provided_hash = req.get_param_value("hash");
      std::string calculated_hash =
          generateHash(file_props.getFileUniqueId(), 6);
      if (provided_hash != calculated_hash) {
        res.status = 403;
        res.set_content("Invalid hash", "text/plain");
        return;
      }
    }

    // Set response headers
    res.set_header("Content-Type", file_props.getMimeType());
    res.set_header("Content-Disposition",
                   "inline; filename=\"" + file_props.getFileName() + "\"");
    res.set_header("Accept-Ranges", "bytes");

    // Handle range request
    int64_t offset = 0;
    int64_t limit = 0;
    if (req.has_header("Range")) {
      std::string range = req.get_header_value("Range");
      size_t dash_pos = range.find('-');
      if (dash_pos != std::string::npos) {
        offset = std::stoll(range.substr(6, dash_pos - 6));
        if (dash_pos + 1 < range.length()) {
          limit = std::stoll(range.substr(dash_pos + 1)) - offset + 1;
        }
      }
      res.status = 206;
    }

    // Stream file
    byte_streamer_->streamFile(
        file_props, [&res](const std::string& chunk) { res.body += chunk; },
        offset, limit);

  } catch (const std::exception& e) {
    spdlog::error("Error handling stream request: {}", e.what());
    res.status = 500;
    res.set_content("Internal server error", "text/plain");
  }
}

std::string Server::generateHash(const std::string& unique_id, int length) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, unique_id.c_str(), unique_id.length());
  SHA256_Final(hash, &sha256);

  std::stringstream ss;
  for (int i = 0; i < length; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(hash[i]);
  }
  return ss.str();
}

}  // namespace server
}  // namespace tglink2aria2