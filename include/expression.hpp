#pragma once

#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include "environment.hpp"
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
    expr_type type;

    std::variant<expr_value, expr_symbol, expr_list> as;

    // std::shared_ptr<environment> lookup = {};

    value eval(std::shared_ptr<environment> lookup) {
        switch (type) {
            case (expr_type::VALUE):
                return std::get<expr_value>(as).val;
            case (expr_type::SYMBOL): {
                return lookup->get(std::string(std::get<expr_symbol>(as).symbol));
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
                    return eval_if(list, lookup);
                } else if (symbol == "define") {
                    return eval_define(list, lookup);
                } else if (symbol == "lambda") {
                    return eval_lambda(list, lookup);
                }

                value symbol_val = lookup->get(std::string(symbol));

                if (symbol_val.type != value_type::function) {
                    std::cout << symbol_val << std::endl;
                    return value{error::inavlid_args, "Invalid Arguments: symbol does not evaluate to a function"};
                }

                std::function<value(std::vector<value>&, std::shared_ptr<environment>)> fn = std::get<function_v>(symbol_val.as).fn;

                std::vector<value> args;
                for (size_t i = 1; i < list.size(); i++) {
                    args.push_back(list[i].eval(lookup));
                }
                return fn(args, lookup);
            }
            default:
                return value{error::unsupported_expr, "Unsupported Expression: invalid expression type"};
        }
    }

   private:
    value eval_if(std::vector<expr>& list, std::shared_ptr<environment> lookup) {
        if (list.size() != 4) {
            return value{error::inavlid_args, "Invalid Arguments: symbol 'if' requires 3 arguments"};
        }
        value cond = list[1].eval(lookup);

        if (cond.type != value_type::boolean) {
            return value{error::inavlid_args, "Invalid Arguments: first argument of 'if' must evaluate to a boolean"};
        }

        if (std::get<boolean_v>(cond.as).val) {
            return list[2].eval(lookup);
        }
        return list[3].eval(lookup);
    }

    value eval_define(std::vector<expr>& list, std::shared_ptr<environment> lookup) {
        if (list.size() != 3) {
            return value{error::inavlid_args, "Invalid Arguments: symbol 'define' requires 2 arguments"};
        }

        if (list[1].type != expr_type::SYMBOL) {
            return value{error::inavlid_args, "Invalid Arguments: first argument of 'define' must be a symbol"};
        }

        std::string_view symbol = std::get<expr_symbol>(list[1].as).symbol;

        value val = list[2].eval(lookup);
        value ret = lookup->emplace(std::string(symbol), val);
        return ret;
    }

    value eval_lambda(std::vector<expr>& list, std::shared_ptr<environment> lookup) {
        if (list.size() != 3) {
            return value{error::inavlid_args, "Invalid Argumnets: symbol 'lambda' requires 2 arguments"};
        }

        if (list[1].type != expr_type::LIST) {
            return value{error::inavlid_args, "Invalid Arguments: first argument of 'lambda' must be a symbol"};
        }

        std::vector<expr> args = std::get<expr_list>(list[1].as).elements;

        std::vector<std::string> args_symbols;

        for (const expr& e : args) {
            if (e.type != expr_type::SYMBOL) {
                return value{error::inavlid_args, "Invalid Arguments: all elements of 'lambda' arguments list must be symbols"};
            }

            std::string arg_symbol = std::string(std::get<expr_symbol>(e.as).symbol);

            if (lookup->contains(arg_symbol)) {
                return value{error::inavlid_args, "Invalid Arguments: cannot use a bound symbol as an argument for 'lambda'"};
            }

            args_symbols.push_back(std::move(arg_symbol));
        }

        expr func_expr = list[2];

        auto func = [args_symbols, func_expr](std::vector<value>& args, std::shared_ptr<environment> lookup) -> value {
            if (args_symbols.size() != args.size()) {
                return value{error::inavlid_args, std::format("Invalid Arguments: expected {} arguments, but recieved {}", args_symbols.size(), args.size())};
            }

            std::shared_ptr<environment> envr = std::make_shared<environment>(lookup);
            auto local_expr = func_expr;

            for (size_t i = 0; i < args.size(); i++) {
                envr->emplace(args_symbols[i], args[i]);
            }

            return local_expr.eval(envr);
        };

        return value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(func)};
    }
};
