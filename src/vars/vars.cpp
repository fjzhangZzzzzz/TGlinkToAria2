#include "vars.hpp"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace tglink2aria2 {
namespace vars {

std::unique_ptr<Vars> Vars::instance_ = nullptr;

Vars& Vars::getInstance() {
  if (!instance_) {
    instance_ = std::unique_ptr<Vars>(new Vars());
  }
  return *instance_;
}

Vars::Vars() { loadFromEnv(); }

void Vars::loadFromEnv() {
  try {
    dotenv::init();

    // Required variables
    api_id_ = std::stoi(dotenv::getenv("API_ID"));
    api_hash_ = dotenv::getenv("API_HASH");
    bot_token_ = dotenv::getenv("BOT_TOKEN");
    bin_channel_ = std::stoll(dotenv::getenv("BIN_CHANNEL"));

    // Optional variables with defaults
    port_ = dotenv::getenv("PORT", "8080").empty()
                ? 8080
                : std::stoi(dotenv::getenv("PORT"));
    bind_address_ = dotenv::getenv("BIND_ADDRESS", "0.0.0.0");
    proxy_ = dotenv::getenv("PROXY", "");
    aria2_enabled_ = dotenv::getenv("ARIA2", "false") == "true";
    rpc_urls_ = dotenv::getenv("RPC_URLS", "");
    rpc_ports_ = dotenv::getenv("RPC_PORTS", "6800").empty()
                     ? 6800
                     : std::stoi(dotenv::getenv("RPC_PORTS"));
    rpc_tokens_ = dotenv::getenv("RPC_TOKENS", "");

    spdlog::info("Configuration loaded successfully");
  } catch (const std::exception& e) {
    spdlog::error("Failed to load configuration: {}", e.what());
    throw;
  }
}

}  // namespace vars
}  // namespace tglink2aria2