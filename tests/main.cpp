#include "logging.h"
#include <gtest/gtest.h>

int main(int argc, char** argv) {
  // Init logger and remove sinks.
  // The logger will offer the normal functionality but it wont print into the console.
  logsys::init();
  logsys::get()->sinks().clear();

  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  spdlog::shutdown(); // flushes all sinks
  return result;
}