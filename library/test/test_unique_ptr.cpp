#include "ptt3/unique_ptr.h"

#include <type_traits>

#include "gtest/gtest.h"

using namespace ptt3::forbidden;

#define UNUSED(x) ((void)(x))

class C
{
public:
  explicit C(bool& b) : isAlive(b) { isAlive = true; }

  ~C() { isAlive = false; }

  const C* address() const { return this; }

private:
  bool& isAlive;
};

TEST(sanity_tests, lifetime_is_recorded)
{
  bool isAlive = false;
  {
    C c(isAlive);
    EXPECT_TRUE(isAlive);
  }
  EXPECT_FALSE(isAlive);
}

TEST(unique_ptr, is_default_constructible)
{
  unique_ptr<int> p;

  UNUSED(p);
}

TEST(unique_ptr, can_contain_pointer)
{
  bool isAlive = false;
  {
    unique_ptr p(new C(isAlive));
    EXPECT_TRUE(isAlive);
    UNUSED(p);
  }
  EXPECT_FALSE(isAlive);
}

static_assert(!std::is_copy_constructible<unique_ptr<int>>::value);
static_assert(!std::is_copy_assignable<unique_ptr<int>>::value);

TEST(unique_ptr, is_move_constructible)
{
  bool isAlive = false;
  unique_ptr p1(new C(isAlive));
  {
    unique_ptr p2(std::move(p1));
    EXPECT_TRUE(isAlive);
    UNUSED(p2);
  }
  EXPECT_FALSE(isAlive);
}

TEST(unique_ptr, is_move_assignable)
{
  bool isAlive1 = false;
  bool isAlive2 = false;
  unique_ptr p1(new C(isAlive1));
  {
    unique_ptr p2(new C(isAlive2));
    p1 = std::move(p2);
    EXPECT_FALSE(isAlive1);
  }
  EXPECT_TRUE(isAlive2);
}

TEST(unique_ptr, can_be_reset)
{
  bool isAlive = false;
  unique_ptr p(new C(isAlive));
  EXPECT_TRUE(isAlive);
  p.reset();
  EXPECT_FALSE(isAlive);
}

TEST(unique_ptr, can_be_reset_with_value)
{
  bool isAlive1 = false;
  unique_ptr p(new C(isAlive1));
  EXPECT_TRUE(isAlive1);

  bool isAlive2 = false;
  p.reset(new C(isAlive2));
  EXPECT_FALSE(isAlive1);
  EXPECT_TRUE(isAlive2);
}

TEST(unique_ptr, is_convertible_to_bool)
{
  const unique_ptr<int> p1;
  const unique_ptr<int> p2(new int);

  bool b = p1;

  EXPECT_FALSE(p1);
  EXPECT_FALSE(b);
  EXPECT_TRUE(p2);
}

TEST(unique_ptr, is_dereferencable)
{
  bool isAlive = false;
  C* rp = new C(isAlive);
  const unique_ptr<C> p(rp);

  EXPECT_EQ(rp, p.get());
  EXPECT_EQ(rp, &*p);
  EXPECT_EQ(rp, (*p).address());
  EXPECT_EQ(rp, p->address());
}
