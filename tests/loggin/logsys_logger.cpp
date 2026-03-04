#include <gtest/gtest.h>

#include "logging.h"

TEST(Logsys_logger, logsys_object_creation) {
  logsys::init();
  EXPECT_NE(logsys::get(), nullptr);
}

TEST(Logsys_logger, duplicated_initialization_of_logger) {
  logsys::init();
  std::shared_ptr<spdlog::logger> logger_1 = logsys::get();
  logsys::init();
  std::shared_ptr<spdlog::logger> logger_2 = logsys::get();
  EXPECT_EQ(logger_1, logger_2);
}