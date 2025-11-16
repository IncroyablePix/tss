#pragma once

#include <string>
#include <memory>
#include <tss/tokenization/TokenValue.h>

namespace Trema::Style
{
    enum class VariableType
    {
        Number,
        Bool,
        String,
    };

    class Variable final : public std::enable_shared_from_this<Variable>
    {
    public:
        explicit Variable(Value value);
        Variable(const Variable&) = delete;
        Variable& operator=(const Variable&) = delete;
        ~Variable();

        Value CopyValue() const;
        static Value CopyValue(Value v) ;

        [[nodiscard]] Value GetValue() const { return m_value; }
        [[nodiscard]] VariableType GetType() const;

        std::string GetIdentity() const;

        friend std::ostream& operator<<(std::ostream& os, const Variable& st);

    private:
        Value m_value;
    };
}
