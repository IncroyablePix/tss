#pragma once

#include <ostream>
#include <string>
#include <tss/tokenization/TokenType.h>
#include <tss/tokenization/TokenValue.h>

namespace Trema
{
    namespace Style
    {
        class Token final
        {
        public:
            Token(TokenType tokenType, unsigned int position, unsigned int line, Value value);

            Token(Token&& other) noexcept;
            Token& operator=(Token&& other) noexcept;

            ~Token();
            [[nodiscard]] std::string GetIdentity() const;
            [[nodiscard]] unsigned int GetPosition() const { return m_position; }
            [[nodiscard]] unsigned int GetLine() const { return m_line; }
            [[nodiscard]] TokenType GetTokenType() const { return m_tokenType; }
            [[nodiscard]] Value GetValue() const { return m_value; }
            [[nodiscard]] std::string ValueAsString() const;

            friend std::ostream &operator<<(std::ostream &os, const Token &token);

        protected:
            TokenType m_tokenType;
            Value m_value;
            unsigned int m_position;
            unsigned int m_line;
        };
    }
}