#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace logsys {
void init();
std::shared_ptr<spdlog::logger> get();
} // namespace logsys
