#pragma once

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>

#include <ptt3/shared_ptr.h>

namespace ptt3::forbidden
{
  class broken_promise : public std::exception
  {
  };

  namespace Detail
  {
    class future_shared_data_base
    {
    public:
      future_shared_data_base() : has_promise_(false), has_value_(false) {}

      void does_have_promise()
      {
        std::unique_lock l(m);
        has_promise_ = true;
      }

      void doesnt_have_promise()
      {
        std::unique_lock l(m);
        has_promise_ = false;
        c.notify_all();
      }

      bool valid()
      {
        std::unique_lock l(m);
        return has_promise_ || has_value_;
      }

      bool has_value() const
      {
        std::unique_lock l(m);
        throw_if_invalid();
        return has_value_;
      }

      void wait() const
      {
        std::unique_lock l(m);
        while (!has_value_ && has_promise_)
        {
          c.wait(l);
        }

        throw_if_invalid();
      }

      template <class Rep, class Period>
      bool wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const
      {
        auto timeout_time = std::chrono::system_clock::now() + timeout_duration;

        std::unique_lock l(m);

        while (!has_value_ && has_promise_ &&
               std::cv_status::no_timeout == c.wait_until(l, timeout_time))
        {
        }
        throw_if_invalid();

        return has_value_;
      }

      template <class Clock, class Duration>
      bool wait_until(
          const std::chrono::time_point<Clock, Duration>& timeout_time) const
      {
        std::unique_lock l(m);

        while (!has_value_ && has_promise_ &&
               std::cv_status::no_timeout == c.wait_until(l, timeout_time))
        {
        }
        throw_if_invalid();

        return has_value_;
      }

      void set()
      {
          std::unique_lock l(m);
          assert(!has_value_);
          has_value_ = true;
          c.notify_all();
      }

    private:
      void throw_if_invalid() const
      {
        if (!has_promise_ && !has_value_) throw broken_promise();
      }

    protected:
      mutable std::mutex m;

    private:
      mutable std::condition_variable c;
      bool has_promise_;
      bool has_value_;
    };

    template <typename T>
    class future_shared_data : public future_shared_data_base
    {
    public:
      T get()
      {
        wait();

        std::unique_lock l(m);
        assert(value);
        return *value;
      }

      void set(T&& v)
      {
          value = std::move(v);
          future_shared_data_base::set();
      }

    private:
      std::optional<T> value;
    };

    template <>
    class future_shared_data<void> : public future_shared_data_base
    {
    public:
      void get()
      {
        wait();
      }
    };
  }  // namespace Detail

  template <typename T>
  class future;

  template <typename T>
  class promise
  {
  public:
    promise() : shared(new Detail::future_shared_data<T>)
    {
      shared->does_have_promise();
    }

    ~promise()
    {
      shared->doesnt_have_promise();
    }

    promise(const promise&) = delete;
    promise& operator=(const promise&) = delete;

    promise(promise&& other) : shared(other.shared)
    {
      other.shared.reset(new Detail::future_shared_data<T>);
      other.shared->does_have_promise();
    }

    promise& operator=(promise&& other)
    {
      std::swap(shared, other.shared);

      return *this;
    }

    void set(T&& v) const
    {
        shared->set(std::move(v));
    }

  private:
    shared_ptr<Detail::future_shared_data<T>> shared;

    friend class future<T>;
  };

  template <>
  class promise<void>
  {
  public:
    promise() : shared(new Detail::future_shared_data<void>)
    {
      shared->does_have_promise();
    }

    ~promise()
    {
      shared->doesnt_have_promise();
    }

    promise(const promise&) = delete;
    promise& operator=(const promise&) = delete;

    promise(promise&& other) : shared(other.shared)
    {
      other.shared.reset(new Detail::future_shared_data<void>);
      other.shared->does_have_promise();
    }

    promise& operator=(promise&& other)
    {
      std::swap(shared, other.shared);
      return *this;
    }

    void set() const
    {
        shared->set();
    }

  private:
    shared_ptr<Detail::future_shared_data<void>> shared;

    friend class future<void>;
  };

  template <typename T>
  class future
  {
  public:
    future() : shared(new Detail::future_shared_data<T>) {}
    explicit future(const promise<T>& p) : shared(p.shared) {}
    future& operator=(const promise<T>& p) { shared = p.shared; return *this; }

    bool valid() const noexcept { return shared->valid(); }
    bool has_value() const { return shared->has_value(); }
    T get() const { return shared->get(); }
    void wait() const { shared->wait(); }

    template <class Rep, class Period>
    bool wait_for(
        const std::chrono::duration<Rep, Period>& timeout_duration) const
    {
      return shared->wait_for(timeout_duration);
    }

    template <class Clock, class Duration>
    bool wait_until(
        const std::chrono::time_point<Clock, Duration>& timeout_time) const
    {
      return shared->wait_until(timeout_time);
    }

  private:
    shared_ptr<Detail::future_shared_data<T>> shared;
  };

}  // namespace ptt3::forbidden
