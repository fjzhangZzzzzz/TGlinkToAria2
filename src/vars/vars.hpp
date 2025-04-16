#pragma once

#include <dotenv/dotenv.h>

#include <memory>
#include <string>
#include <vector>

namespace tglink2aria2 {
namespace vars {

class Vars {
 public:
  static Vars& getInstance();

  // Getters
  int getApiId() const { return api_id_; }
  const std::string& getApiHash() const { return api_hash_; }
  const std::string& getBotToken() const { return bot_token_; }
  int64_t getBinChannel() const { return bin_channel_; }
  int getPort() const { return port_; }
  const std::string& getBindAddress() const { return bind_address_; }
  const std::string& getProxy() const { return proxy_; }
  bool isAria2Enabled() const { return aria2_enabled_; }
  const std::string& getRpcUrls() const { return rpc_urls_; }
  int getRpcPorts() const { return rpc_ports_; }
  const std::string& getRpcTokens() const { return rpc_tokens_; }

 private:
  Vars();
  ~Vars() = default;
  Vars(const Vars&) = delete;
  Vars& operator=(const Vars&) = delete;

  void loadFromEnv();

  int api_id_;
  std::string api_hash_;
  std::string bot_token_;
  int64_t bin_channel_;
  int port_;
  std::string bind_address_;
  std::string proxy_;
  bool aria2_enabled_;
  std::string rpc_urls_;
  int rpc_ports_;
  std::string rpc_tokens_;

  static std::unique_ptr<Vars> instance_;
};

}  // namespace vars
}  // namespace tglink2aria2