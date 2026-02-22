#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace logsys {
inline std::string loginPattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v";
void configureShinks();
void init();
std::shared_ptr<spdlog::logger> get();
} // namespace logsys
