#pragma once

/*!
 * \file
 *
 * \brief This header defines macros for constructor forwarding and inheriting.
 *
 * These macros can be used as inheriting constructors
 * from C++11 (not implemented in most compilers) or to forward
 * constructor arguments to member. Constructor's inheriting declaration
 * will have noexcept specifier as target's constructor,
 * i.e. iff target's constructor is noexcept, declared constructor
 * is also noexcept.
 *
 * \warning Target constructor has to be public (for inheriting macros).
 */

#include <utility>

#define BUNSAN_INHERIT_ATTRIBUTES_CONSTRUCTOR(ATTRIBUTES, CLASS, PARENT) \
    template <typename ... Args> \
    ATTRIBUTES CLASS(Args &&...args) noexcept(noexcept(PARENT(std::forward<Args>(args)...))): \
        PARENT(std::forward<Args>(args)...) {}

#define BUNSAN_INHERIT_CONSTRUCTOR(CLASS, PARENT) \
    BUNSAN_INHERIT_ATTRIBUTES_CONSTRUCTOR(, CLASS, PARENT)

#define BUNSAN_INHERIT_EXPLICIT_CONSTRUCTOR(CLASS, PARENT) \
    BUNSAN_INHERIT_ATTRIBUTES_CONSTRUCTOR(explicit, CLASS, PARENT)

#define BUNSAN_FORWARD_ATTRIBUTES_CONSTRUCTOR(ATTRIBUTES, CLASS, PARENT) \
    template <typename ... Args> \
    ATTRIBUTES CLASS(Args &&...args): \
        PARENT(std::forward<Args>(args)...) {}

#define BUNSAN_FORWARD_CONSTRUCTOR(CLASS, PARENT) \
    BUNSAN_FORWARD_ATTRIBUTES_CONSTRUCTOR(, CLASS, PARENT)

#define BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(CLASS, PARENT) \
    BUNSAN_FORWARD_ATTRIBUTES_CONSTRUCTOR(explicit, CLASS, PARENT)
