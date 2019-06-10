#include "helpers.h"

#include "gtest/gtest.h"

RecordLifetime::RecordLifetime(bool& b) : isAlive(b) { isAlive = true; }

RecordLifetime::~RecordLifetime() { isAlive = false; }

const RecordLifetime* RecordLifetime::address() const { return this; }

TEST(sanity_tests, lifetime_is_recorded)
{
  bool isAlive = false;
  {
    RecordLifetime c(isAlive);
    EXPECT_TRUE(isAlive);
  }
  EXPECT_FALSE(isAlive);
}

