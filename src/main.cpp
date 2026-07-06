#include <charconv>
#include <expected>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "expression.hpp"

std::expected<std::unique_ptr<primative>, error> parse_primative(std::string_view token);
std::expected<std::unique_ptr<binary>, error> parse_binary(std::string_view op_token, std::string_view arg1_token, std::string_view arg2_token);
std::expected<std::unique_ptr<expression>, error> parse_expression(std::string_view token);

std::expected<std::unique_ptr<primative>, error> parse_primative(std::string_view token) {
    int res{};

    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), res);

    if (ptr != token.data() + token.size()) {
        return std::unexpected(error::bad_format);
    }

    if (ec != std::errc()) {
        return std::unexpected(error::bad_format);
    }

    return std::make_unique<primative>(res);
}

std::expected<std::unique_ptr<binary>, error> parse_binary(std::string_view op_token, std::string_view arg1_token, std::string_view arg2_token) {
    auto lres = parse_expression(arg1_token);
    if (!lres) {
        return std::unexpected(lres.error());
    }

    auto rres = parse_expression(arg2_token);
    if (!rres) {
        return std::unexpected(rres.error());
    }

    binary_operation op{binary_operation::nullop};

    if (op_token == "+") {
        op = binary_operation::addition;
    } else if (op_token == "-") {
        op = binary_operation::subtraction;
    } else if (op_token == "*") {
        op = binary_operation::multiplication;
    } else if (op_token == "/") {
        op = binary_operation::division;
    }

    if (op == binary_operation::nullop) return std::unexpected(error::unsupported_expr);

    return std::make_unique<binary>(op, std::move(lres.value()), std::move(rres.value()));
}

std::expected<std::unique_ptr<expression>, error> parse_expression(std::string_view token) {
    if (token[0] != '(') {
        return std::unexpected(error::bad_format);
    }

    std::size_t end = token.find_first_of(" ");
    if (end == std::string_view::npos) {
        return std::unexpected(error::bad_format);
    }

    std::string_view op_token = token.substr(1, end - 1);

    if (std::isdigit(static_cast<unsigned char>(op_token[0]))) {
        auto res = parse_primative(op_token);
        if (!res) return std::unexpected(res.error());

        // check that parens close

        return std::unique_ptr<expression>(res.value().release());
    }

    // a better way to do this is find out how many terms there are before close paren!
    std::cout << op_token << std::endl;

    if (op_token == "+" || op_token == "-" || op_token == "*" || op_token == "/") {
        std::size_t start = end;
        end = token.find_first_of(" ", start);
        if (end == std::string_view::npos) {
            return std::unexpected(error::bad_format);
        }
        std::string_view arg1_token = token.substr(start, end);
        std::cout << arg1_token << " len:" << arg1_token.length() << std::endl;

        start = end;
        end = token.find_first_of(" )", start);
        if (end == std::string_view::npos) {
            return std::unexpected(error::bad_format);
        }
        std::string_view arg2_token = token.substr(start, end);
        std::cout << arg2_token << " len:" << arg2_token.length() << std::endl;

        // check that parens close

        auto res = parse_binary(op_token, arg1_token, arg2_token);
        if (!res) return std::unexpected(res.error());

        return std::unique_ptr<expression>(res.value().release());
    }

    return std::unexpected(error::unsupported_expr);
}

int main() {
    std::string ln{};

    while (std::getline(std::cin, ln)) {
        auto eval = parse_expression(ln);
        if (!eval) {
            std::cout << eval.error() << std::endl;
        } else {
            auto val = eval.value()->eval();
            if (!val) {
                std::cout << val.error() << std::endl;
            } else {
                std::cout << "> " << val.value() << std::endl;
            }
        }
    }

    return 0;
}