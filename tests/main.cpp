#include <cstdio>
#include <gtest/gtest.h>

int main(int argc, char** argv) {
  printf("test binary started\n");
  fflush(stdout);
  ::testing::InitGoogleTest(&argc, argv);
  printf("gtest initialized\n");
  fflush(stdout);
  return RUN_ALL_TESTS();
}