#pragma once

namespace ptt3::forbidden
{
  template <typename T>
  class unique_ptr
  {
  public:
    unique_ptr() : p(nullptr) {}
    explicit unique_ptr(T* other) : p(other) {}
    unique_ptr(const unique_ptr<T>&) = delete;
    unique_ptr<T>& operator=(const unique_ptr<T>&) = delete;
    ~unique_ptr() { delete p; }

    unique_ptr(unique_ptr<T>&& other) : p(other.p) { other.p = nullptr; }

    unique_ptr<T>& operator=(unique_ptr<T>&& other)
    {
        reset(other.p);
        other.p = nullptr;

        return *this;
    }

    void reset(T* other = nullptr)
    {
        delete p;
        p = other;
    }

    operator bool() const { return p; }

    T* get() const { return p; }
    T& operator*() const { return *p; }
    T* operator->() const { return p; }

  private:
    T* p;
  };
}  // namespace ptt3::forbidden
