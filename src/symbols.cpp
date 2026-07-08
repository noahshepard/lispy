#include "symbols.hpp"

#include "expression.hpp"
#include "value.hpp"

std::unordered_map<std::string_view, std::function<value(std::vector<value>)>> init_symbols() {
    std::unordered_map<std::string_view, std::function<value(std::vector<value>)>> lookup;

    lookup["+"] = [](std::vector<value> args) -> value {
        if (args.size() < 2) {
            return value{error::inavlid_args, "Invalid Arguments: symbol '+' must have >1 arguments"};
        }

        int sum = 0;
        for (value x : args) {
            if (x.type == value_type::number) {
                sum += std::get<number_v>(x.as).num;
            } else if (x.type == value_type::error) {
                return x;
            } else {
                return value{error::inavlid_args, "Invalid Arguments: non-numerical symbol for '+'"};
            }
        }
        return value{sum};
    };

    lookup["-"] = [](std::vector<value> args) -> value {
        if (args.size() == 0) {
            return value{error::inavlid_args, "Invalid Arguments: symbol '-' must have >0 arguments"};
        }

        if (args[0].type != value_type::number) {
            return (args[0].type == value_type::error) ? args[0] : value{error::inavlid_args, "Invalid Arguments: non-numerical symbol for '-"};
        }

        if (args.size() == 1) {
            return value{-std::get<number_v>(args[0].as).num};
        }

        int diff = std::get<number_v>(args[0].as).num;
        for (size_t i = 1; i < args.size(); i++) {
            if (args[i].type == value_type::number) {
                diff -= std::get<number_v>(args[i].as).num;
            } else if (args[i].type == value_type::error) {
                return args[i];
            } else {
                return value{error::inavlid_args, "Invalid Arguments: non-numerical symbol for '-'"};
            }
        }
        return value{diff};
    };

    lookup["*"] = [](std::vector<value> args) -> value {
        if (args.size() < 2) {
            return value{error::inavlid_args, "Invalid Arguments: symbol '*' must have >1 arguments"};
        }

        int product = 1;
        for (value x : args) {
            if (x.type == value_type::number) {
                product *= std::get<number_v>(x.as).num;
            } else if (x.type == value_type::error) {
                return x;
            } else {
                return value{error::inavlid_args, "Invalid Arguments: non-numerical symbol for '*'"};
            }
        }
        return value{product};
    };

    lookup["/"] = [](std::vector<value> args) -> value {
        if (args.size() < 2) {
            return value{error::inavlid_args, "Invalid Arguments: symbol '/' must have >1 arguments"};
        }

        if (args[0].type != value_type::number) {
            return (args[0].type == value_type::error) ? args[0] : value{error::inavlid_args, "Invalid Arguments: non-numerical symbol for '-"};
        }

        int dividend = std::get<number_v>(args[0].as).num;
        for (size_t i = 1; i < args.size(); i++) {
            if (args[i].type == value_type::number) {
                int divisor = std::get<number_v>(args[i].as).num;
                if (divisor == 0) return value{error::divide_by_zero, "Cannot Divide By Zero"};
                dividend /= divisor;
            } else if (args[i].type == value_type::error) {
                return args[i];
            } else {
                return value{error::inavlid_args, "Invalid Arguments: non-numerical symbol for '*'"};
            }
        }
        return value{dividend};
    };

    lookup[">"] = [](std::vector<value> args) -> value {
        if (args.size() != 2) {
            return value{error::inavlid_args, "Invalid Arguments: symbol '>' reqires 2 arguments"};
        }

        if (args[0].type != value_type::number || args[1].type != value_type::number) {
            return value{error::inavlid_args, "Inavlid Arguments: non-numerical symbol for '>'"};
        }
        int a = std::get<number_v>(args[0].as).num;
        int b = std::get<number_v>(args[1].as).num;
        return value{a > b};
    };

    return lookup;
}