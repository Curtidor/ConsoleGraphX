#pragma once
#include <cassert>
#include <iostream>

namespace ConsoleGraphX_Internal
{
#ifdef NDEBUG
#define CGX_VERIFY(...) ((void)0)
#else
    // Internal helper macro to handle zero or one message argument
#define CGX_VERIFY_IMPL(ptr, msg) \
        do { \
            if (!(ptr)) { \
                std::cerr << "Assertion failed: " << (msg) << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
                assert((ptr) != nullptr); \
            } \
        } while (0)

    // Macro to check if the number of arguments is one or two
#define GET_MACRO(_1, _2, NAME, ...) NAME

// Public macro that calls the helper with either one or two arguments
#define CGX_VERIFY(...) GET_MACRO(__VA_ARGS__, CGX_VERIFY_IMPL2, CGX_VERIFY_IMPL1)(__VA_ARGS__)

// One argument version uses a default message
#define CGX_VERIFY_IMPL1(ptr) CGX_VERIFY_IMPL(ptr, "Pointer must not be null")

// Two arguments version uses the provided message
#define CGX_VERIFY_IMPL2(ptr, msg) CGX_VERIFY_IMPL(ptr, msg)
#endif
}
