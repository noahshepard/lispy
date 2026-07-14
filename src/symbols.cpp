#include "symbols.hpp"

#include "environment.hpp"
#include "expression.hpp"
#include "value.hpp"

value addition(const std::vector<value>& args, const std::shared_ptr<environment> lookup) {
    (void)lookup;
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
}

value substraction(const std::vector<value>& args, const std::shared_ptr<environment> lookup) {
    (void)lookup;
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
}

value multiplication(const std::vector<value>& args, const std::shared_ptr<environment> lookup) {
    (void)lookup;
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
}

value division(const std::vector<value>& args, const std::shared_ptr<environment> lookup) {
    (void)lookup;
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
}

value greater_than(const std::vector<value>& args, const std::shared_ptr<environment> lookup) {
    (void)lookup;
    if (args.size() != 2) {
        return value{error::inavlid_args, "Invalid Arguments: symbol '>' reqires 2 arguments"};
    }

    if (args[0].type != value_type::number || args[1].type != value_type::number) {
        return value{error::inavlid_args, "Inavlid Arguments: non-numerical symbol for '>'"};
    }
    int a = std::get<number_v>(args[0].as).num;
    int b = std::get<number_v>(args[1].as).num;
    return value{a > b};
}

value equal_to(const std::vector<value>& args, const std::shared_ptr<environment> lookup) {
    (void)lookup;
    if (args.size() < 2) {
        return value{error::inavlid_args, "Invalid Arguments: symbol '=' requires >1 arguments"};
    }

    for (size_t i = 1; i < args.size(); i++) {
        if (args[i].type != args[i - 1].type) {
            return value{false};
        }

        value_type t = args[i].type;

        switch (t) {
            case value_type::boolean:
                if (std::get<boolean_v>(args[i].as).val != std::get<boolean_v>(args[i - 1].as).val) {
                    return value{false};
                }
                break;
            case value_type::error:
                if (std::get<error_v>(args[i].as).err != std::get<error_v>(args[i - 1].as).err) {
                    return value{false};
                }
                break;
            case value_type::function:
                return value{false};
            case value_type::number:
                if (std::get<number_v>(args[i].as).num != std::get<number_v>(args[i - 1].as).num) {
                    return value{false};
                }
                break;
            case value_type::symbol:
                if (std::get<symbol_v>(args[i].as).str != std::get<symbol_v>(args[i - 1].as).str) {
                    return value{false};
                }
                break;
            default:
                return value{false};
        }
    }
    return value{true};
}

std::shared_ptr<environment> init_symbols() {
    std::shared_ptr<environment> lookup = std::make_shared<environment>();

    lookup->emplace("+", value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(addition)});

    lookup->emplace("-", value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(substraction)});

    lookup->emplace("*", value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(multiplication)});

    lookup->emplace("/", value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(division)});

    lookup->emplace(">", value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(greater_than)});

    lookup->emplace("true", value{true});
    lookup->emplace("false", value{false});

    lookup->emplace("=", value{std::function<value(std::vector<value>&, std::shared_ptr<environment>)>(equal_to)});

    return lookup;
}