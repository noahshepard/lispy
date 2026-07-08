#include <charconv>
#include <expected>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "expression.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

constexpr std::string symbol_to_string(symbol_type t) {
    switch (t) {
        case (symbol_type::PLUS):
            return "+";
        case (symbol_type::MINUS):
            return "+";
        case (symbol_type::TIMES):
            return "+";
        case (symbol_type::DIVIDE):
            return "+";
        default:
            return "null symbol";
    }
}

constexpr std::string expr_to_string(expr e) {
    switch (e.type) {
        case (expr_type::NUMBER):
            return std::to_string(std::get<expr_number>(e.as).number);
        case (expr_type::SYMBOL):
            return symbol_to_string(std::get<expr_symbol>(e.as).type);
        case (expr_type::LIST): {
            auto& list = std::get<expr_list>(e.as);
            std::string ret = "(list: ";
            for (const auto& le : list.elements) {
                ret += expr_to_string(le);
                ret += ", ";
            }
            ret += ")";
            return ret;
        }
        default:
            return "error: unknown type";
    }
}

std::ostream& operator<<(std::ostream& os, expr e) {
    return os << expr_to_string(e);
}

int main() {
    std::string raw_ln{};

    while (std::getline(std::cin, raw_ln)) {
        std::vector<token> tokens = tokenize(raw_ln);

        for (const token& t : tokens) {
            std::cout << "type: " << t.type << " | src: " << t.str << std::endl;
        }

        parser p{std::move(tokens)};
        expr e = p.parse();

        std::cout << "> " << e << std::endl;
    }

    return 0;
}
