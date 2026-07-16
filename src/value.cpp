#include "value.hpp"

#include <string>

std::string value_to_string(value v) {
    switch (v.type) {
        case (value_type::number):
            return std::to_string(std::get<number_v>(v.as).num);
        case (value_type::symbol):
            return std::string(std::get<symbol_v>(v.as).str);
        case (value_type::function):
            return "[function]";
        case (value_type::boolean):
            return std::get<boolean_v>(v.as).val ? "true" : "false";
        case (value_type::error):
            return std::string(std::get<error_v>(v.as).msg);
        case (value_type::list): {
            list_v l = std::get<list_v>(v.as);
            std::string ret = "[";
            while (!l.empty()) {
                ret += value_to_string(l.head->car);
                ret += ", ";
                l = l.head->cdr;
            }
            ret += "]";
            return ret;
        }
    }
}

std::ostream& operator<<(std::ostream& os, value v) {
    return os << value_to_string(v);
}
