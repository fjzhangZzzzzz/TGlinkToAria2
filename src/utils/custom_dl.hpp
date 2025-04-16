#pragma once

#include <tgbot/tgbot.h>

#include <functional>
#include <memory>
#include <string>

#include "file_properties.hpp"

namespace tglink2aria2 {
namespace utils {

class ByteStreamer {
 public:
  ByteStreamer(TgBot::Bot& bot);
  ~ByteStreamer() = default;

  // Stream file in chunks
  void streamFile(const FileProperties& file_props,
                  std::function<void(const std::string& chunk)> callback,
                  int64_t offset = 0, int64_t limit = 0);

 private:
  TgBot::Bot& bot_;
};

}  // namespace utils
}  // namespace tglink2aria2