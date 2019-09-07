#include "ptt3/future.h"

#include "gtest/gtest.h"

#include <thread>

#include "helpers.h"

using namespace ptt3::forbidden;

const auto timeout = std::chrono::milliseconds(500);

using system_clock = std::chrono::system_clock;

namespace
{
  void is_not_valid(const future<int>& f)
  {
    EXPECT_FALSE(f.valid());
    EXPECT_THROW(f.has_value(), broken_promise);
    EXPECT_THROW(f.get(), broken_promise);
    EXPECT_THROW(f.wait(), broken_promise);
    EXPECT_THROW(f.wait_for(timeout), broken_promise);
    EXPECT_THROW(f.wait_until(system_clock::now() + timeout), broken_promise);
  }

  void is_not_valid(const future<void>& f)
  {
    EXPECT_FALSE(f.valid());
    EXPECT_THROW(f.has_value(), broken_promise);
    EXPECT_THROW(f.get(), broken_promise);
    EXPECT_THROW(f.wait(), broken_promise);
    EXPECT_THROW(f.wait_for(timeout), broken_promise);
    EXPECT_THROW(f.wait_until(system_clock::now() + timeout), broken_promise);
  }

  void is_valid(const future<int>& f, int expectedValue)
  {
    EXPECT_TRUE(f.valid());
    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.wait_for(timeout));
    EXPECT_TRUE(f.wait_until(system_clock::now() + timeout));
    f.wait();
    EXPECT_EQ(expectedValue, f.get());
  }

  void is_valid(const future<void>& f)
  {
    EXPECT_TRUE(f.valid());
    EXPECT_TRUE(f.has_value());
    EXPECT_TRUE(f.wait_for(timeout));
    EXPECT_TRUE(f.wait_until(system_clock::now() + timeout));
    f.wait();
  }
}

TEST(future, is_default_constructible)
{
  future<int> f;

  is_not_valid(f);
}

TEST(future, is_default_constructible_for_void)
{
  future<void> f;

  is_not_valid(f);
}

TEST(future, is_constructible_from_promise)
{
    promise<int> p;
    future f(p);

    p.set(5);

    is_valid(f, 5);
}

TEST(future, is_constructible_from_promise_for_void)
{
    promise<void> p;
    future f(p);

    p.set();

    is_valid(f);
}

static_assert(!std::is_copy_constructible<promise<int>>::value);
static_assert(!std::is_copy_assignable<promise<int>>::value);
static_assert(!std::is_copy_constructible<promise<void>>::value);
static_assert(!std::is_copy_assignable<promise<void>>::value);

TEST(future, is_valid_after_promise_is_gone)
{
   future<int> f;
   {
       promise<int> p;
       f = p;
       p.set(5);
   }

   is_valid(f, 5);
}

TEST(future, is_valid_after_promise_is_gone_for_void)
{
   future<void> f;
   {
       promise<void> p;
       f = p;
       p.set();
   }

   is_valid(f);
}

TEST(future, is_copyable)
{
    promise<int> p;
    future f(p);
    future f2(f);

    p.set(5);

    is_valid(f, 5);
    is_valid(f2, 5);
}

TEST(future, is_copyable_for_void)
{
    promise<void> p;
    future f(p);
    future f2(f);

    p.set();

    is_valid(f);
    is_valid(f2);
}

TEST(future, times_out_if_promise_unset)
{
  promise<int> p;
  future f(p);

  promise<void> pHelper;
  future fHelper(pHelper);

  EXPECT_FALSE(f.wait_for(timeout));
  EXPECT_FALSE(f.wait_until(std::chrono::system_clock::now() + timeout));

  std::thread t([&] {
    f.wait();
    pHelper.set();
  });
  t.detach();
  EXPECT_FALSE(fHelper.wait_for(timeout));
  p.set(5);
  EXPECT_TRUE(fHelper.wait_for(timeout));
  EXPECT_EQ(5, f.get());
}

TEST(promise, is_move_constructible)
{
  promise<int> p;
  future f(p);
  {
    promise p2(std::move(p));
  }

  is_not_valid(f);
}

TEST(promise, is_move_constructible_for_void)
{
  promise<void> p;
  future f(p);
  {
    promise p2(std::move(p));
  }

  is_not_valid(f);
}

TEST(promise, is_move_assignable)
{
  promise<int> p;
  future f(p);
  {
    promise<int> p2;
    p2 = std::move(p);
  }

  is_not_valid(f);
}

TEST(promise, is_move_assignable_for_void)
{
  promise<void> p;
  future f(p);
  {
    promise<void> p2;
    p2 = std::move(p);
  }

  is_not_valid(f);
}
