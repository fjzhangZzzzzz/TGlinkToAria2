#pragma once

#include <tgbot/types/Message.h>

#include <memory>
#include <string>

namespace tglink2aria2 {
namespace utils {

class FileProperties {
 public:
  FileProperties(const TgBot::Message::Ptr& message);
  ~FileProperties() = default;

  // Getters
  const std::string& getFileId() const { return file_id_; }
  const std::string& getFileUniqueId() const { return file_unique_id_; }
  int64_t getFileSize() const { return file_size_; }
  const std::string& getMimeType() const { return mime_type_; }
  const std::string& getFileName() const { return file_name_; }

 private:
  void parseFromMessage(const TgBot::Message::Ptr& message);

  std::string file_id_;
  std::string file_unique_id_;
  int64_t file_size_;
  std::string mime_type_;
  std::string file_name_;
};

}  // namespace utils
}  // namespace tglink2aria2