#include "ptt3/shared_ptr.h"
#include "ptt3/shared_ptr.h"

#include <type_traits>

#include "gtest/gtest.h"

#include "helpers.h"

using namespace ptt3::forbidden;

TEST(shared_ptr, is_default_constructible)
{
  shared_ptr<int> p;

  UNUSED(p);
}

TEST(shared_ptr, can_contain_pointer)
{
  bool isAlive = false;
  {
    shared_ptr p(new RecordLifetime(isAlive));
    EXPECT_TRUE(isAlive);
    UNUSED(p);
  }
  EXPECT_FALSE(isAlive);
}

TEST(shared_ptr, is_copy_constructible)
{
  bool isAlive = false;
  {
    shared_ptr p1(new RecordLifetime(isAlive));
    {
      shared_ptr p2(p1);
      EXPECT_TRUE(isAlive);
      UNUSED(p2);
    }
    EXPECT_TRUE(isAlive);
  }
  EXPECT_FALSE(isAlive);
}

TEST(shared_ptr, is_copy_assignable)
{
  bool isAlive2 = false;
  {
    bool isAlive1 = false;
    shared_ptr p1(new RecordLifetime(isAlive1));
    {
      shared_ptr p2(new RecordLifetime(isAlive2));
      p1 = p2;
      EXPECT_FALSE(isAlive1);
    }
    EXPECT_TRUE(isAlive2);
  }
  EXPECT_FALSE(isAlive2);
}



TEST(shared_ptr, can_be_reset)
{
  bool isAlive = false;
  shared_ptr p(new RecordLifetime(isAlive));
  EXPECT_TRUE(isAlive);
  p.reset();
  EXPECT_FALSE(isAlive);
}

TEST(shared_ptr, can_be_reset_with_value)
{
  bool isAlive1 = false;
  shared_ptr p(new RecordLifetime(isAlive1));
  EXPECT_TRUE(isAlive1);

  bool isAlive2 = false;
  p.reset(new RecordLifetime(isAlive2));
  EXPECT_FALSE(isAlive1);
  EXPECT_TRUE(isAlive2);
}

TEST(shared_ptr, is_convertible_to_bool)
{
  const shared_ptr<int> p1;
  const shared_ptr<int> p2(new int);

  bool b = p1;

  EXPECT_FALSE(p1);
  EXPECT_FALSE(b);
  EXPECT_TRUE(p2);
}

TEST(shared_ptr, is_dereferencable)
{
  bool isAlive = false;
  RecordLifetime* rp = new RecordLifetime(isAlive);
  const shared_ptr<RecordLifetime> p(rp);

  EXPECT_EQ(rp, p.get());
  EXPECT_EQ(rp, &*p);
  EXPECT_EQ(rp, (*p).address());
  EXPECT_EQ(rp, p->address());
}
