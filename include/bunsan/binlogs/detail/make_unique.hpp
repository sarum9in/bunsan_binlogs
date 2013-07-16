#pragma once

#include <memory>
#include <utility>

namespace bunsan {
namespace binlogs {
namespace detail {

template <typename T, typename ... Args>
std::unique_ptr<T> make_unique(Args &&...args)
{
    std::unique_ptr<T> ptr(new T(std::forward<Args>(args)...));
    return ptr;
}

}
}
}
