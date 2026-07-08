#pragma once

#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>

#include "value.hpp"

struct expr;

enum class expr_type {
    VALUE,
    SYMBOL,
    LIST,
};

struct expr_value {
    value val;
};

struct expr_symbol {
    std::string_view symbol;
};

struct expr_list {
    std::vector<expr> elements;
};

struct expr {
    static std::unordered_map<std::string_view, std::function<value(std::vector<value>)>> lookup;

    expr_type type;

    std::variant<expr_value, expr_symbol, expr_list> as;

    value eval() {
        switch (type) {
            case (expr_type::VALUE):
                return std::get<expr_value>(as).val;
            case (expr_type::SYMBOL):
                return value{error::unsupported_expr, "Unsupported Expression: symbol without arguments"};
            case (expr_type::LIST): {
                std::vector<expr>& list = std::get<expr_list>(as).elements;

                if (list.empty()) {
                    return value{error::unsupported_expr, "Unsupported Expression: empty list"};
                }

                if (list[0].type != expr_type::SYMBOL) {
                    return value{error::unsupported_expr, "Unsupported Expression: first argument must be symbol"};
                }

                auto it = lookup.find(std::get<expr_symbol>(list[0].as).symbol);
                if (it == lookup.end()) {
                    return value{error::unsupported_expr, "Unsupported Expression: unreckonized symbol"};
                }

                std::function<value(std::vector<value>)> fn = it->second;

                std::vector<value> args;
                for (size_t i = 1; i < list.size(); i++) {
                    args.push_back(list[i].eval());
                }
                return fn(args);
            }
            default:
                return value{error::unsupported_expr, "Unsupported Expression: invalid expression type"};
        }
    }
};
