#include <spdlog/spdlog.h>

#include <csignal>
#include <iostream>

#include "bot/bot.hpp"

namespace {
std::unique_ptr<tglink2aria2::bot::Bot> bot;
volatile std::sig_atomic_t running = 1;

void signalHandler(int signal) { running = 0; }
}  // namespace

int main() {
  try {
    // Setup signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Setup logging
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

    // Create and start bot
    bot = std::make_unique<tglink2aria2::bot::Bot>();
    bot->start();

    // Main loop
    while (running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Stop bot
    bot->stop();

  } catch (const std::exception& e) {
    spdlog::error("Fatal error: {}", e.what());
    return 1;
  }

  return 0;
}