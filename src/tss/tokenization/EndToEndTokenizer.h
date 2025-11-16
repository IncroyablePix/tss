#pragma once

#include <deque>
#include <tss/tokenization/ITokenizer.h>
#include <tss/tokenization/TokenType.h>
#include <tss/errors/MistakesContainer.h>

namespace Trema
{
    namespace Style
    {
        class EndToEndTokenizer final : public ITokenizer
        {
        public:
            explicit EndToEndTokenizer(std::string code, MistakesContainer& mistakes);
            EndToEndTokenizer(const EndToEndTokenizer&) = delete;
            EndToEndTokenizer& operator=(const EndToEndTokenizer&) = delete;

            EndToEndTokenizer(EndToEndTokenizer&&) noexcept = default;
            EndToEndTokenizer& operator=(EndToEndTokenizer&&) noexcept = default;

            ~EndToEndTokenizer() override = default;
            Token GetNextToken() override;
            [[nodiscard]] bool Empty() const override  { return m_tokens.empty(); }
            [[nodiscard]] size_t Size() const override { return m_tokens.size(); }
        private:
            TokenType m_lastType { TokenType::LeftParenthesis };
            std::deque<Token> m_tokens;
            std::string m_codeStorage; // Owns the string
            std::string_view m_code;    // Views the string
            unsigned int m_cursor { 0 };
            unsigned int m_line { 1 };
            unsigned int m_linePos { 1 };
            Token ParseToken(MistakesContainer& mistakes);
            // --- Helper methods for token parsing ---
            void SkipWhitespace(unsigned int& pos);
            Token ParseSingleCharToken(unsigned int& pos, TokenType type);
            Token ParseStringLiteral(unsigned int& pos, MistakesContainer& mistakes);
            Token ParseComment(unsigned int& pos);
            Token ParseOperator(unsigned int& pos);
            Token ParseIdentifier(unsigned int& pos);
            Token ParseNumber(unsigned int& pos);
            void HandleUnknownToken(unsigned int& pos, MistakesContainer& mistakes);

            [[nodiscard]] static bool IsOperator(char c);
            [[nodiscard]] static bool IsAllowedIdentifierChar(char c);
            [[nodiscard]] static bool IsAllowedIdentifierStartChar(char c);
            [[nodiscard]] static Integer IntFromHex(std::string_view string);
            [[nodiscard]] static bool IsBoolValue(std::string_view string);
            [[nodiscard]] static bool IsHexNumber(std::string_view string);
            [[nodiscard]] static bool IsFloatNumber(std::string_view string, TokenType lastType);
            [[nodiscard]] static bool IsCommentEnd(std::string_view string);
            [[nodiscard]] static bool IsCommentStart(std::string_view string);
        };
    }
}