#pragma once

#include <cstddef>
#include <functional>

namespace ptt3::forbidden
{
  namespace Detail
  {
    inline void DoNothing() {}

    class shared_data
    {
    public:
      explicit shared_data(std::function<void()>&& deleter_ = DoNothing);
      ~shared_data();

      void AddRef();
      void Release();

      template <typename T>
      static shared_data* create(T* p)
      {
        return new shared_data([p]() { delete p; });
      }

      static shared_data* create();

    private:
      size_t refCount;
      std::function<void()> deleter;
    };

  }  // namespace Detail

  template <typename T>
  class shared_ptr
  {
  public:
    shared_ptr() : p(nullptr), shared(Detail::shared_data::create())
    {
      shared->AddRef();
    }

    explicit shared_ptr(T* other)
        : p(other), shared(Detail::shared_data::create(other))
    {
        shared->AddRef();
    }

    ~shared_ptr() { shared->Release(); }

    shared_ptr(const shared_ptr<T>& other) : p(other.p), shared(other.shared)
    {
      shared->AddRef();
    }

    shared_ptr<T>& operator=(const shared_ptr<T>& other)
    {
        shared->Release();

        p = other.p;
        shared = other.shared;
        shared->AddRef();

        return *this;
    }

    void reset(T* other = nullptr)
    {
        shared->Release();

        p = other;
        shared = Detail::shared_data::create(p);
        shared->AddRef();
    }

    operator bool() const { return p; }

    T* get() const { return p; }
    T& operator*() const { return *p; }
    T* operator->() const { return p; }

  private:
    T* p;
    Detail::shared_data* shared;
  };
}  // namespace ptt3::forbidden
