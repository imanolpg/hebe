#include "logging.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace {
std::shared_ptr<spdlog::logger> g_logger;
}

namespace logsys {

void configureShinks() {

  auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  std::vector<spdlog::sink_ptr> sinks{console};
  g_logger = std::make_shared<spdlog::logger>("hebe", sinks.begin(), sinks.end());

  // Default level: info (use debug while developing)
  g_logger->set_level(spdlog::level::info);
  g_logger->flush_on(spdlog::level::warn);

  // nice readable format
  g_logger->set_pattern(logsys::loginPattern);
  spdlog::set_default_logger(g_logger);
}

void init() {
  if (g_logger)
    return;

  configureShinks();
}

std::shared_ptr<spdlog::logger> get() {
  return g_logger ? g_logger : spdlog::default_logger();
}

} // namespace logsys