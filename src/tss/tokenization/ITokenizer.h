#pragma once

#include <tss/tokenization/Token.h>

namespace Trema
{
    namespace Style
    {
        class ITokenizer
        {
        public:
            virtual ~ITokenizer() = default;
            virtual Token GetNextToken() = 0;
            [[nodiscard]] virtual bool Empty() const = 0;
            [[nodiscard]] virtual size_t Size() const = 0;
        };
    }
}