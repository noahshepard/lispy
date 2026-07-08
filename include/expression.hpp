#pragma once

#include <expected>
#include <memory>
#include <variant>

#include "error.hpp"

enum class expr_type {
    NUMBER,
    SYMBOL,
    LIST
};

enum class symbol_type {
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    NULL_TYPE
};

struct expr;

struct expr_number {
    int number;
};

struct expr_symbol {
    symbol_type type;
};

struct expr_list {
    std::vector<expr> elements;
};

struct expr {
    expr_type type;

    std::variant<expr_number, expr_symbol, expr_list> as;
};
