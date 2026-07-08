#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <variant>

enum class error {
    divide_by_zero,
    bad_eval,
    bad_format,
    unsupported_expr,
    paren_miss,
    inavlid_args,
};

enum class value_type {
    number,
    function,
    symbol,
    boolean,
    error
};

struct value;

struct number_v {
    int num;
};

struct function_v {
    std::function<value(std::vector<value>)> fn;
};

struct symbol_v {
    std::string_view str;
};

struct boolean_v {
    bool val;
};

struct error_v {
    error err;
    std::string_view msg;

    explicit error_v(error e) : err(e), msg("") {}
    explicit error_v(error e, std::string_view m) : err(e), msg(m) {}
};

struct value {
    value_type type;

    std::variant<number_v, function_v, symbol_v, boolean_v, error_v> as;

    explicit value(int num) : type(value_type::number), as(number_v{num}) {}
    explicit value(std::function<value(std::vector<value>)> fn) : type(value_type::function), as(function_v{fn}) {}
    explicit value(std::string_view str) : type(value_type::symbol), as(symbol_v{str}) {}
    explicit value(bool val) : type(value_type::boolean), as(boolean_v{val}) {}
    explicit value(error err) : type(value_type::error), as(error_v{err}) {}
    explicit value(error err, std::string_view msg) : type(value_type::error), as(error_v{err, msg}) {}
};

std::string value_to_string(value v);
std::ostream& operator<<(std::ostream& os, value v);