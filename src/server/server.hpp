#pragma once

#include <httplib.h>

#include <functional>
#include <memory>
#include <string>

#include "utils/custom_dl.hpp"
#include "utils/file_properties.hpp"

namespace tglink2aria2 {
namespace server {

class Server {
 public:
  Server(const std::string& bind_address, int port, TgBot::Bot& bot);
  ~Server();

  void start();
  void stop();

 private:
  void setupRoutes();
  void handleStream(const httplib::Request& req, httplib::Response& res);
  std::string generateHash(const std::string& unique_id, int length);

  std::string bind_address_;
  int port_;
  TgBot::Bot& bot_;
  std::unique_ptr<httplib::Server> server_;
  std::unique_ptr<utils::ByteStreamer> byte_streamer_;
};

}  // namespace server
}  // namespace tglink2aria2