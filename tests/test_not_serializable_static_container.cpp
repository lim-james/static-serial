#include "static_serial_test.hpp"

/// open to re-exploring this in the future for C support
/// currently is_serializable decays T - char[] -> char* 
/// but currently in alignment w/ modern C++ opt for 
/// std::array instead

static_assert(stse::is_serializable_v<char[1]>           == false);
static_assert(stse::is_serializable_v<decltype("Hello")> == false);
