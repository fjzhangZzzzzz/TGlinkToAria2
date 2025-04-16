#include "file_properties.hpp"

#include <spdlog/spdlog.h>

namespace tglink2aria2 {
namespace utils {

FileProperties::FileProperties(const TgBot::Message::Ptr& message) {
  parseFromMessage(message);
}

void FileProperties::parseFromMessage(const TgBot::Message::Ptr& message) {
  if (message->document) {
    file_id_ = message->document->fileId;
    file_unique_id_ = message->document->fileUniqueId;
    file_size_ = message->document->fileSize;
    mime_type_ = message->document->mimeType;
    file_name_ = message->document->fileName;
  } else if (message->video) {
    file_id_ = message->video->fileId;
    file_unique_id_ = message->video->fileUniqueId;
    file_size_ = message->video->fileSize;
    mime_type_ = "video/mp4";
    file_name_ = "video.mp4";
  } else if (message->audio) {
    file_id_ = message->audio->fileId;
    file_unique_id_ = message->audio->fileUniqueId;
    file_size_ = message->audio->fileSize;
    mime_type_ = message->audio->mimeType;
    file_name_ = message->audio->fileName.empty() ? "audio.mp3"
                                                  : message->audio->fileName;
  } else if (message->voice) {
    file_id_ = message->voice->fileId;
    file_unique_id_ = message->voice->fileUniqueId;
    file_size_ = message->voice->fileSize;
    mime_type_ = "audio/ogg";
    file_name_ = "voice.ogg";
  } else if (message->videoNote) {
    file_id_ = message->videoNote->fileId;
    file_unique_id_ = message->videoNote->fileUniqueId;
    file_size_ = message->videoNote->fileSize;
    mime_type_ = "video/mp4";
    file_name_ = "video_note.mp4";
  } else if (message->photo.size() > 0) {
    // Get the largest photo
    const auto& photo = message->photo.back();
    file_id_ = photo->fileId;
    file_unique_id_ = photo->fileUniqueId;
    file_size_ = photo->fileSize;
    mime_type_ = "image/jpeg";
    file_name_ = "photo.jpg";
  } else if (message->sticker) {
    file_id_ = message->sticker->fileId;
    file_unique_id_ = message->sticker->fileUniqueId;
    file_size_ = message->sticker->fileSize;
    mime_type_ = "image/webp";
    file_name_ = "sticker.webp";
  } else {
    throw std::runtime_error("Unsupported message type");
  }

  spdlog::debug(
      "Parsed file properties: id={}, unique_id={}, size={}, mime={}, name={}",
      file_id_, file_unique_id_, file_size_, mime_type_, file_name_);
}

}  // namespace utils
}  // namespace tglink2aria2