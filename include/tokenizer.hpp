#pragma once

#include <regex>
#include <string_view>

enum class token_type {
    WHITESPACE,
    L_PAREN,
    R_PAREN,
    SYMBOL_NUMBER,
    END
};

constexpr std::string_view token_to_string(token_type t);

std::ostream& operator<<(std::ostream& os, token_type t);

struct token {
    token_type type;
    std::string_view str;
};

struct token_rule {
    token_type type;
    std::regex pattern;
};

std::vector<token> tokenize(const std::string& src);
