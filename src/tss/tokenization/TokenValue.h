#pragma once
#include <cstdint>
#include <optional>
#include <variant>
#include <string>

namespace Trema
{
    namespace Style
    {
        using Float = double;
        using Integer = int64_t;

        using Value = std::variant<Float, Integer, bool, std::string, std::nullopt_t>;
        using TokenValue = Value;

        std::string GetIdentity(const Value &tokenValue);
    }
}