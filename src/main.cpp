#include <charconv>
#include <expected>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "environment.hpp"
#include "expression.hpp"
#include "parser.hpp"
#include "symbols.hpp"
#include "tokenizer.hpp"

constexpr std::string expr_to_string(expr e) {
    switch (e.type) {
        case (expr_type::VALUE):
            return value_to_string(std::get<expr_value>(e.as).val);
        case (expr_type::SYMBOL):
            return std::string(std::get<expr_symbol>(e.as).symbol);
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
    std::shared_ptr<environment> lookup = init_symbols();
    std::string raw_ln{};

    while (std::getline(std::cin, raw_ln)) {
        std::vector<token> tokens = tokenize(raw_ln);

        // for (const token& t : tokens) {
        //     std::cout << "type: " << t.type << " | src: " << t.str << std::endl;
        // }

        parser p{std::move(tokens)};
        expr e = p.parse();

        std::cout << "> " << e.eval(lookup) << std::endl;

        // lookup->print_envr();
    }
    return 0;
}
