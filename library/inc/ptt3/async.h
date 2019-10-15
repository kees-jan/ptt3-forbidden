#pragma once

#include <type_traits>
#include <utility>
#include <thread>

#include <ptt3/future.h>

namespace ptt3::forbidden
{
    template<typename Func>
    auto async(Func func)
    {
        promise<typename std::invoke_result<Func>::type> p;
        future f(p);

        std::thread t([p = std::move(p), func = std::forward<decltype(func)>(func)] { p.set(func()); });
        t.detach();
        return f;
    }
}
