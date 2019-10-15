#include "ptt3/async.h"

#include "gtest/gtest.h"

#include <thread>

#include "helpers.h"

using namespace ptt3::forbidden;

const auto timeout = std::chrono::milliseconds(500);

TEST(async, does_stuff_asynchronously)
{
  promise<void> pHelper;
  future fHelper(pHelper);

  auto f = async([&] {
    fHelper.wait();
    return 5;
  });

  EXPECT_FALSE(f.wait_for(timeout));
  pHelper.set();
  EXPECT_TRUE(f.wait_for(timeout));
  EXPECT_EQ(5, f.get());
}
