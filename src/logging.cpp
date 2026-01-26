#include "logging.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace {
  std::shared_ptr<spdlog::logger> g_logger;
}

namespace logsys {

void init() {
  if (g_logger) return;

  auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file    = std::make_shared<spdlog::sinks::basic_file_sink_mt>("hebe.log", true);

  std::vector<spdlog::sink_ptr> sinks { console, file };
  g_logger = std::make_shared<spdlog::logger>("hebe", sinks.begin(), sinks.end());

  // default level: info (use debug while developing)
  g_logger->set_level(spdlog::level::info);
  g_logger->flush_on(spdlog::level::warn);

  // nice readable format
  g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
  spdlog::set_default_logger(g_logger);
}

std::shared_ptr<spdlog::logger> get() {
  return g_logger ? g_logger : spdlog::default_logger();
}

} // namespace logsys