#include <charconv>
#include <iostream>
#include <vector>

#include "expression.hpp"
#include "tokenizer.hpp"

class parser {
   public:
    explicit parser(std::vector<token> tokens) : pos_(0), tokens_(std::move(tokens)) {}

    expr parse() {
        if (tokens_[pos_].type == token_type::L_PAREN) {
            return parse_list();
        }

        if (tokens_[pos_].type == token_type::SYMBOL_NUMBER) {
            return parse_atom();
        }

        return {};
    }

   private:
    size_t pos_;
    std::vector<token> tokens_;

    expr parse_list() {
        if (tokens_[pos_++].type != token_type::L_PAREN) {
            std::cout << "no l paren!" << std::endl;
        }
        expr e;
        e.type = expr_type::LIST;
        e.as = expr_list{};
        auto& list = std::get<expr_list>(e.as);

        while (tokens_[pos_].type != token_type::R_PAREN) {
            list.elements.push_back(this->parse());
        }

        if (tokens_[pos_++].type != token_type::R_PAREN) {
            std::cout << "no r paren!" << std::endl;
        }

        return e;
    }

    expr parse_atom() {
        token t = tokens_[pos_++];
        expr e;

        int val;
        auto [ptr, ec] = std::from_chars(t.str.data(), t.str.data() + t.str.size(), val);

        if (ec == std::errc{} && ptr == t.str.data() + t.str.size()) {
            e.type = expr_type::NUMBER;
            e.as = expr_number{val};
        } else {
            e.type = expr_type::SYMBOL;
            e.as = expr_symbol{lookup_symbol(t.str)};
        }
        return e;
    }

    symbol_type lookup_symbol(std::string_view symbol) {
        if (symbol == "+") return symbol_type::PLUS;
        if (symbol == "-") return symbol_type::MINUS;
        if (symbol == "*") return symbol_type::TIMES;
        if (symbol == "/") return symbol_type::DIVIDE;
        return symbol_type::NULL_TYPE;
    }
};