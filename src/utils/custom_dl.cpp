#include "custom_dl.hpp"

#include <spdlog/spdlog.h>

namespace tglink2aria2 {
namespace utils {

ByteStreamer::ByteStreamer(TgBot::Bot& bot) : bot_(bot) {}

void ByteStreamer::streamFile(
    const FileProperties& file_props,
    std::function<void(const std::string& chunk)> callback, int64_t offset,
    int64_t limit) {
  try {
    spdlog::debug("Starting to stream file: id={}, offset={}, limit={}",
                  file_props.getFileId(), offset, limit);

    // Get file from Telegram
    auto file = bot_.getApi().getFile(file_props.getFileId());
    if (!file) {
      throw std::runtime_error("Failed to get file from Telegram");
    }

    // Calculate chunk size
    const int64_t chunk_size = 1024 * 1024;  // 1MB
    int64_t remaining = limit > 0 ? limit : file->fileSize - offset;
    int64_t current_offset = offset;

    while (remaining > 0) {
      int64_t current_chunk_size = std::min(remaining, chunk_size);

      // Download chunk
      auto chunk = bot_.getApi().downloadFile(file->filePath, current_offset,
                                              current_chunk_size);
      if (chunk.empty()) {
        throw std::runtime_error("Failed to download file chunk");
      }

      // Call callback with chunk
      callback(chunk);

      // Update counters
      current_offset += current_chunk_size;
      remaining -= current_chunk_size;
    }

    spdlog::debug("Finished streaming file: id={}", file_props.getFileId());
  } catch (const std::exception& e) {
    spdlog::error("Error while streaming file: {}", e.what());
    throw;
  }
}

}  // namespace utils
}  // namespace tglink2aria2