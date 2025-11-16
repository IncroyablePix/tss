#include <format>
#include <sstream>
#include <tss/variables/Variable.h>
#include <tss/tokenization/TokenValue.h>
#include <tss/utils/StringConversion.h>
#include <iomanip>
#include <iostream>
#include <utility>

namespace Trema::Style
{
    Variable::Variable(Value value) :
        m_value(std::move(value))
    {
    }

    VariableType Variable::GetType() const
    {
        return std::visit([]<typename T0>(T0&&) -> VariableType
        {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, Float>)
                return VariableType::Number;
            else if constexpr (std::is_same_v<T, Integer>)
                return VariableType::Number;
            else if constexpr (std::is_same_v<T, bool>)
                return VariableType::Bool;
            else
                throw std::runtime_error("Unsupported variable type");
        }, m_value);
    }

    std::string Variable::GetIdentity() const
    {
        return std::format("Variable({})", Style::GetIdentity(m_value));
    }

    std::ostream &operator<<(std::ostream &os, const Variable &st)
    {
        os << st.GetIdentity();
        return os;
    }

    Variable::~Variable()
    = default;

    Value Variable::CopyValue() const
    {
        return CopyValue(m_value);
    }

    Value Variable::CopyValue(Value v)
    {
        return v;
    }
}
