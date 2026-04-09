#include "static_serial.hpp"

#include <string>
#include <vector>

static_assert(stse::is_serializable_v<std::string>      == false);
static_assert(stse::is_serializable_v<std::vector<int>> == false);
