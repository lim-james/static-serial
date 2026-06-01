#include "static_serial_test.hpp"

struct NotTriviallyCopyable { 
    int a, b; 
    NotTriviallyCopyable(const NotTriviallyCopyable&) {}
};

static_assert(std::is_trivially_copyable_v<NotTriviallyCopyable> == false);
static_assert(stse::Serializable<NotTriviallyCopyable>           == false);
