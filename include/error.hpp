#pragma once

enum class error {
    divide_by_zero,
    bad_eval,
    bad_format,
    unsupported_expr,
    paren_miss,
    inavlid_args,
};

/*constexpr std::string_view to_string(error err) {
    switch (err) {
        case (error::divide_by_zero):
            return "divide by zero";
        case (error::bad_eval):
            return "bad eval";
        case (error::bad_format):
            return "bad format";
        case (error::unsupported_expr):
            return "unsupported expr";
        case (error::paren_miss):
            return "paren_miss";
        case (error::inavlid_args):
            return "invalid args";
    }
}

std::ostream& operator<<(std::ostream& os, error err) {
    return os << to_string(err);
}*/