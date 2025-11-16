#include <tss/tokenization/TokenValue.h>

namespace Trema
{
    namespace Style
    {
        std::string GetIdentity(const Value& tokenValue)
        {
            return std::visit(
                []<typename T0>(T0&& arg) -> std::string
                {
                    using T = std::decay_t<T0>;
                    if constexpr (std::is_same_v<T, Float>)
                        return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, Integer>)
                        return std::to_string(arg);
                    else if constexpr (std::is_same_v<T, bool>)
                        return arg ? "true" : "false";
                    else if constexpr (std::is_same_v<T, std::string>)
                        return arg;
                    else
                        return "null";
                },
                tokenValue
            );
        }
    }
}
