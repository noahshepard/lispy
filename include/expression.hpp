#include <expected>
#include <memory>

enum class binary_operation {
    addition,
    subtraction,
    multiplication,
    division,
    nullop
};

enum class error {
    divide_by_zero,
    bad_eval,
    bad_format,
    unsupported_expr,
    paren_miss,
    inavlid_args,
};

constexpr std::string_view to_string(error err) {
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
}

class expression {
   public:
    virtual std::expected<int, error> eval() const = 0;

    virtual ~expression() = default;
};

class primative : public expression {
   public:
    primative(int value) : value_(value) {}

    std::expected<int, error> eval() const override {
        return value_;
    }

   private:
    int value_;
};

/*class unary : public expression {
   public:
    unary(std::function<int(const expression&)> func, expression* arg) : func_(func), arg_(arg) {}

    int eval() const override {
        return func_(*arg_);
    }

   private:
    expression* arg_;
    std::function<int(const expression&)> func_;
};*/

class binary : public expression {
   public:
    binary(binary_operation op, std::unique_ptr<expression> arg1, std::unique_ptr<expression> arg2) : op_(op), arg1_(std::move(arg1)), arg2_(std::move(arg2)) {}

    std::expected<int, error> eval() const override {
        auto l = arg1_->eval();
        if (!l) return l;
        auto r = arg2_->eval();
        if (!r) return r;

        switch (op_) {
            case binary_operation::addition:
                return l.value() + r.value();
            case binary_operation::subtraction:
                return l.value() - r.value();
            case binary_operation::multiplication:
                return l.value() * r.value();
            case binary_operation::division:
                return (r.value() == 0) ? std::unexpected(error::divide_by_zero) : std::expected<int, error>(l.value() / r.value());
            default:
                return std::unexpected(error::bad_eval);
        }
    }

   private:
    std::unique_ptr<expression> arg1_;
    std::unique_ptr<expression> arg2_;
    binary_operation op_;
};