#include "tokenizer.hpp"

#include <iostream>
#include <regex>

constexpr std::string_view token_to_string(token_type t) {
    switch (t) {
        case (token_type::WHITESPACE):
            return "whitespace";
        case (token_type::L_PAREN):
            return "left paren";
        case (token_type::R_PAREN):
            return "right paren";
        case (token_type::SYMBOL_NUMBER):
            return "symbol or number";
        case (token_type::END):
            return "end";
        default:
            return "error: default cause reached?";
    }
}

std::ostream& operator<<(std::ostream& os, token_type t) {
    return os << token_to_string(t);
}

const std::array<token_rule, 4> regex_rules = {{{token_type::WHITESPACE, std::regex(R"(^[\s,]+)")},
                                                {token_type::L_PAREN, std::regex(R"(^\()")},
                                                {token_type::R_PAREN, std::regex(R"(^\))")},
                                                {token_type::SYMBOL_NUMBER, std::regex(R"(^[^\s\[\]{}('"`,;)]+)")}}};

std::vector<token> tokenize(const std::string& src) {
    std::vector<token> tokens;
    size_t pos = 0;

    while (pos < src.size()) {
        bool matched = false;
        const char* start = src.data() + pos;
        const char* end = src.data() + src.size();

        for (const token_rule& rule : regex_rules) {
            std::cmatch match;

            if (std::regex_search(start, end, match, rule.pattern)) {
                size_t len = match[0].length();

                if (rule.type != token_type::WHITESPACE) {
                    tokens.emplace_back(rule.type, std::string_view(start, len));
                }

                pos += len;
                matched = true;
                break;
            }
        }
        if (!matched) {
            std::cout << "Unexpected char at pos: " << std::to_string(pos) << ";" << src[pos] << std::endl;
            ++pos;
        }
    }
    tokens.emplace_back(token_type::END, "");
    return tokens;
}