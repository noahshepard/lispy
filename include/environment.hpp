#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "value.hpp"

class environment {
   public:
    void print_envr() {
        for (const auto& pair : lookup_) {
            std::cout << "key: " << pair.first << " value: " << pair.second << std::endl;
        }
    }

    environment(const std::shared_ptr<environment> parent) : parent_(parent) {
        lookup_ = {};
    }

    environment() {
        parent_ = nullptr;
        lookup_ = {};
    }

    value emplace(const std::string& key, const value& val) {
        auto ret = lookup_.emplace(key, val);
        if (!ret.second) {
            return value{error::redefinition, "Redefinition: symbol already defined"};
        }
        return val;
    }

    bool contains(const std::string& key) const {
        if (!lookup_.contains(key)) {
            return (parent_ == nullptr) ? false : parent_->contains(key);
        }
        return true;
    }

    value get(std::string key) const {
        auto search = lookup_.find(key);
        if (search == lookup_.end()) {
            return (parent_ == nullptr) ? value{error::unsupported_expr, std::format("Unsuported Expression: unknown symbol '{}'", key)} : parent_->get(key);
        }
        return search->second;
    }

   private:
    std::unordered_map<std::string, value> lookup_;
    std::shared_ptr<environment> parent_;
};
