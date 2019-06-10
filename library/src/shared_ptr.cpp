#include <ptt3/shared_ptr.h>

namespace ptt3::forbidden::Detail
{
  shared_data::shared_data(std::function<void()>&& deleter_)
      : refCount(0), deleter(deleter_)
  {
  }
  shared_data::~shared_data() { deleter(); }

  void shared_data::AddRef() { refCount++; }

  void shared_data::Release()
  {
    refCount--;
    if (refCount == 0) delete this;
  }

  shared_data* shared_data::create() { return new shared_data; }
}  // namespace ptt3::forbidden::Detail