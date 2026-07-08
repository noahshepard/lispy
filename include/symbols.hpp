#pragma once
#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "value.hpp"

std::unordered_map<std::string_view, std::function<value(std::vector<value>)>> init_symbols();