#pragma once

#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
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
    static std::unordered_map<std::string, value> lookup;

    expr_type type;

    std::variant<expr_value, expr_symbol, expr_list> as;

    value eval() {
        switch (type) {
            case (expr_type::VALUE):
                return std::get<expr_value>(as).val;
            case (expr_type::SYMBOL): {
                auto it = lookup.find(std::string(std::get<expr_symbol>(as).symbol));
                if (it == lookup.end()) {
                    return value{error::unsupported_expr, "Unsupported Expression: unrecognized symbol"};
                }
                return it->second;
            }
            case (expr_type::LIST): {
                std::vector<expr>& list = std::get<expr_list>(as).elements;

                if (list.empty()) {
                    return value{error::unsupported_expr, "Unsupported Expression: empty list"};
                }

                if (list[0].type != expr_type::SYMBOL) {
                    return value{error::unsupported_expr, "Unsupported Expression: first argument must be symbol"};
                }

                std::string_view symbol = std::get<expr_symbol>(list[0].as).symbol;

                if (symbol == "if") {
                    return eval_if(list);
                } else if (symbol == "define") {
                    return eval_define(list);
                }

                auto it = lookup.find(std::string(symbol));
                if (it == lookup.end()) {
                    return value{error::unsupported_expr, "Unsupported Expression: unrecognized symbol"};
                }

                if (it->second.type != value_type::function) {
                    return value{error::inavlid_args, "Invalid Arguments: symbol does not evaluate to a function"};
                }

                std::function<value(std::vector<value>)> fn = std::get<function_v>(it->second.as).fn;

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

   private:
    value eval_if(std::vector<expr> list) {
        if (list.size() != 4) {
            return value{error::inavlid_args, "Invalid Arguments: symbol 'if' requires 3 arguments"};
        }
        value cond = list[1].eval();

        if (cond.type != value_type::boolean) {
            return value{error::inavlid_args, "Invalid Arguments: first argument of 'if' must evaluate to a boolean"};
        }

        if (std::get<boolean_v>(cond.as).val) {
            return list[2].eval();
        }
        return list[3].eval();
    }

    value eval_define(std::vector<expr> list) {
        if (list.size() != 3) {
            return value{error::inavlid_args, "Invalid Arguments: symbol 'define' requires 2 arguments"};
        }

        if (list[1].type != expr_type::SYMBOL) {
            return value{error::inavlid_args, "Invalid Arguments: first arguments of 'define' must be a symbol"};
        }
        std::string_view symbol = std::get<expr_symbol>(list[1].as).symbol;

        value val = list[2].eval();
        lookup.emplace(std::string(symbol), val);
        return val;
    }
};
