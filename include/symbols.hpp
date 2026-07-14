#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "environment.hpp"
#include "value.hpp"

std::shared_ptr<environment> init_symbols();