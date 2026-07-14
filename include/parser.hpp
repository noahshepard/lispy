#include <charconv>
#include <iostream>
#include <vector>

#include "expression.hpp"
#include "tokenizer.hpp"

class parser {
   public:
    explicit parser(std::vector<token> tokens) : pos_(0), tokens_(std::move(tokens)) {}

    expr parse() {
        if (at_end()) return expr{expr_type::VALUE, expr_value{value{error::bad_format, "Bad Format!"}}};

        if (tokens_[pos_].type == token_type::L_PAREN) {
            return parse_list();
        }

        if (tokens_[pos_].type == token_type::SYMBOL_NUMBER) {
            return parse_atom();
        }

        return {expr_type::VALUE, expr_value{value{error::bad_format, "Invalid Format: expected symbol or list"}}};
    }

   private:
    size_t pos_;
    std::vector<token> tokens_;

    bool at_end() const {
        return pos_ >= tokens_.size() ||
               tokens_[pos_].type == token_type::END;
    }

    expr parse_list() {
        if (tokens_[pos_++].type != token_type::L_PAREN) {
            std::cout << "no l paren!" << std::endl;
        }
        expr e{expr_type::LIST, expr_list{}};

        auto& list = std::get<expr_list>(e.as);

        if (at_end()) {
            return {expr_type::VALUE, expr_value{value{error::bad_format, "Invalid Format: expected symbol or list"}}};
        }

        while (tokens_[pos_].type != token_type::R_PAREN) {
            list.elements.push_back(this->parse());
            if (tokens_[pos_].type == token_type::END) {
                return {expr_type::VALUE, expr_value{value{error::bad_format, "Invalid Format: unexpected end of expression"}}};
            }
        }

        if (tokens_[pos_++].type != token_type::R_PAREN) {
            std::cout << "no r paren!" << std::endl;
        }

        return e;
    }

    expr parse_atom() {
        token t = tokens_[pos_++];
        expr e{expr_type::SYMBOL, expr_value{value{error::inavlid_args, "Invalid Arguments: expected number or symbol"}}};

        int val;
        auto [ptr, ec] = std::from_chars(t.str.data(), t.str.data() + t.str.size(), val);

        if (ec == std::errc{} && ptr == t.str.data() + t.str.size()) {
            e.type = expr_type::VALUE;
            e.as = expr_value{value{val}};
        } else {
            e.type = expr_type::SYMBOL;
            e.as = expr_symbol{t.str};
        }
        return e;
    }
};