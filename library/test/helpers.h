#pragma once

#define UNUSED(x) ((void)(x))

class RecordLifetime
{
public:
  explicit RecordLifetime(bool& b);

  ~RecordLifetime();

  const RecordLifetime* address() const;

private:
  bool& isAlive;
};
