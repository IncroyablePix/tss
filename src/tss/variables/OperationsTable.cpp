#include <stdexcept>
#include <tss/variables/OperationsTable.h>


namespace Trema::Style
{
    OperationsTable::OperationsTable()
    {
        InsertOperator("+", 1, true, [](const Value& a, const Value& b) -> Value
        {
            if (const auto aFloat = std::get_if<Float>(&a))
            {
                if (const auto bFloat = std::get_if<Float>(&b))
                    return *aFloat + *bFloat;
                if (const auto bInteger = std::get_if<Integer>(&b))
                    return *aFloat + static_cast<Float>(*bInteger);
            }
            else if (const auto bFloat = std::get_if<Float>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return static_cast<Float>(*aInteger) + *bFloat;
            }
            else if (const auto bInteger = std::get_if<Integer>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return *aInteger + *bInteger;
            }

            throw std::runtime_error("Unsupported operand types for + operator");
        });

        InsertOperator("-", 1, true, [](const Value& a, const Value& b) -> Value
        {
            if (const auto aFloat = std::get_if<Float>(&a))
            {
                if (const auto bFloat = std::get_if<Float>(&b))
                    return *aFloat - *bFloat;
                if (const auto bInteger = std::get_if<Integer>(&b))
                    return *aFloat - static_cast<Float>(*bInteger);
            }
            else if (const auto bFloat = std::get_if<Float>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return static_cast<Float>(*aInteger) - *bFloat;
            }
            else if (const auto bInteger = std::get_if<Integer>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return *aInteger - *bInteger;
            }

            throw std::runtime_error("Unsupported operand types for - operator");
        });

        InsertOperator("*", 2, true, [](const Value& a, const Value& b) -> Value
        {
            if (const auto aFloat = std::get_if<Float>(&a))
            {
                if (const auto bFloat = std::get_if<Float>(&b))
                    return *aFloat * *bFloat;
                if (const auto bInteger = std::get_if<Integer>(&b))
                    return *aFloat * static_cast<Float>(*bInteger);
            }
            else if (const auto bFloat = std::get_if<Float>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return static_cast<Float>(*aInteger) * *bFloat;
            }
            else if (const auto bInteger = std::get_if<Integer>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return *aInteger * *bInteger;
            }

            throw std::runtime_error("Unsupported operand types for * operator");
        });

        InsertOperator("/", 2, true, [](const Value& a, const Value& b) -> Value
        {
            if (const auto aFloat = std::get_if<Float>(&a))
            {
                if (const auto bFloat = std::get_if<Float>(&b))
                    return *aFloat / *bFloat;
                if (const auto bInteger = std::get_if<Integer>(&b))
                    return *aFloat / static_cast<Float>(*bInteger);
            }
            else if (const auto bFloat = std::get_if<Float>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return static_cast<Float>(*aInteger) / *bFloat;
            }
            else if (const auto bInteger = std::get_if<Integer>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return *aInteger / *bInteger;
            }

            throw std::runtime_error("Unsupported operand types for / operator");
        });

        InsertOperator("%", 2, true, [](const Value& a, const Value& b) -> Value
        {
            if (const auto aFloat = std::get_if<Float>(&a))
            {
                if (const auto bFloat = std::get_if<Float>(&b))
                    return static_cast<Integer>(*aFloat) % static_cast<Integer>(*bFloat);
                if (const auto bInteger = std::get_if<Integer>(&b))
                    return static_cast<Integer>(*aFloat) % *bInteger;
            }
            else if (const auto bFloat = std::get_if<Float>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return *aInteger % static_cast<Integer>(*bFloat);
            }
            else if (const auto bInteger = std::get_if<Integer>(&b))
            {
                if (const auto aInteger = std::get_if<Integer>(&a))
                    return *aInteger % *bInteger;
            }

            throw std::runtime_error("Unsupported operand types for % operator");
        });
    }

    void OperationsTable::InsertOperator(const std::string& name, const int priority, const bool leftAssociative,
                                         std::function<Value(const Value&, const Value&)> operation)
    {
        const OperatorData operatorData
        {
            .Name = name,
            .Priority = priority,
            .IsLeftAssociative = leftAssociative,
            .Operation = std::move(operation)
        };

        m_operators[name] = operatorData;
    }

    const OperatorData& OperationsTable::GetOperator(const std::string& name) const
    {
        return m_operators.at(name);
    }
}
