#include <format>
#include <utility>
#include <tss/tokenization/EndToEndTokenizer.h>

namespace Trema::Style
{
    bool EndToEndTokenizer::IsCommentStart(const std::string_view string)
    {
        if (string.size() < 2)
            return false;

        if (string[0] == '/' && string[1] == '*')
            return true;

        return false;
    }

    bool EndToEndTokenizer::IsCommentEnd(const std::string_view string)
    {
        if (string.size() < 2)
            return false;

        if (string[0] == '*' && string[1] == '/')
            return true;

        return false;
    }

    bool EndToEndTokenizer::IsFloatNumber(const std::string_view string, const TokenType lastType)
    {
        if (string[0] == '\0')
            return false;

        if (std::isdigit(string[0]))
            return true;

        if (string[1] == '\0')
            return false;

        if (string[0] == '.' && std::isdigit(string[1]))
            return true;

        if ((lastType == TokenType::LeftParenthesis || lastType == TokenType::VariableAssignment || lastType ==
            TokenType::PropertyAssignment) && string[0] == '-' && std::isdigit(string[1]))
            return true;

        if (string[2] == '\0')
            return false;

        if ((lastType == TokenType::LeftParenthesis || lastType == TokenType::PropertyAssignment || lastType ==
                TokenType::VariableAssignment) &&
            string[0] == '-' &&
            string[1] == '.' &&
            std::isdigit(string[2]))
            return true;

        if (string[0] && (string[1] == 'x' || string[1] == 'X'))
            return true;

        return false;
    }

    bool EndToEndTokenizer::IsHexNumber(const std::string_view string)
    {
        return string[0] == '0' && (string[1] == 'x' || string[1] == 'X') &&
            string.size() > 2 &&
            string.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
    }

    bool EndToEndTokenizer::IsBoolValue(const std::string_view string)
    {
        const auto len = string.size();
        if (len < 4)
            return false;

        if (string.substr(0, 4) == "true")
            return true;

        if (len >= 5 && string.substr(0, 5) == "false")
            return true;

        return false;
    }

    Integer EndToEndTokenizer::IntFromHex(const std::string_view string)
    {
        const auto hexString = string.substr(2, string.size() - 2);
        const auto val = std::stoll(hexString.data(), nullptr, 16);

        return val;
    }

    bool EndToEndTokenizer::IsAllowedIdentifierStartChar(const char c)
    {
        switch (c)
        {
        case '-':
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '\'':
        case '\n':
        case '\0':
        case '"':
        case ':':
        case ';':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '=':
        case '#':
        case '*':
        case '/':
        case ' ':
        case '\t':
            return false;
        default:
            return true;
        }
    }

    bool EndToEndTokenizer::IsAllowedIdentifierChar(const char c)
    {
        switch (c)
        {
        case '.':
        case '\'':
        case '\n':
        case '\0':
        case '"':
        case ':':
        case ';':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '=':
        case '#':
        case '*':
        case '/':
        case ' ':
        case '\t':
            return false;
        default:
            return true;
        }
    }

    bool EndToEndTokenizer::IsOperator(const char c)
    {
        switch (c)
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            return true;
        }

        return false;
    }

    EndToEndTokenizer::EndToEndTokenizer(std::string code, MistakesContainer& mistakes) :
        m_codeStorage(std::move(code)),
        m_code(m_codeStorage)
    {
        while (m_lastType != TokenType::EndOfCode)
        {
            m_tokens.push_back(ParseToken(mistakes));
        }
    }

    Token EndToEndTokenizer::ParseToken(MistakesContainer& mistakes)
    {
        unsigned int pos = m_cursor;
        SkipWhitespace(pos);
        m_cursor = pos;
        while (pos < m_code.size())
        {
            char c = m_code[pos];
            if (c == ';')
                return ParseSingleCharToken(pos, TokenType::EndOfInstruction);
            if (c == '(')
                return ParseSingleCharToken(pos, TokenType::LeftParenthesis);
            if (c == ')')
                return ParseSingleCharToken(pos, TokenType::RightParenthesis);
            if (c == '{')
                return ParseSingleCharToken(pos, TokenType::LeftCurlyBracket);
            if (c == '}')
                return ParseSingleCharToken(pos, TokenType::RightCurlyBracket);
            if (c == ':')
                return ParseSingleCharToken(pos, TokenType::PropertyAssignment);
            if (c == '=')
                return ParseSingleCharToken(pos, TokenType::VariableAssignment);
            if (c == '#')
                return ParseSingleCharToken(pos, TokenType::Identity);
            if (c == '\'' || c == '"')
                return ParseStringLiteral(pos, mistakes);
            if (IsCommentStart(m_code.substr(pos)))
                return ParseComment(pos);
            if (IsOperator(c))
                return ParseOperator(pos);
            if (IsAllowedIdentifierStartChar(c))
                return ParseIdentifier(pos);
            if (IsFloatNumber(m_code.substr(pos), m_lastType))
                return ParseNumber(pos);

            HandleUnknownToken(pos, mistakes);
        }
        m_cursor = pos + 1;
        m_lastType = TokenType::EndOfCode;
        Token t(TokenType::EndOfCode, m_linePos, m_line, Value{});
        return t;
    }

    void EndToEndTokenizer::SkipWhitespace(unsigned int& pos)
    {
        std::locale loc("C");
        while (pos < m_code.size() && std::isspace(static_cast<unsigned char>(m_code[pos])))
        {
            if (m_code[pos] == '\n')
            {
                m_line++;
                m_linePos = 0;
            }
            m_linePos++;
            pos++;
        }
    }

    Token EndToEndTokenizer::ParseSingleCharToken(unsigned int& pos, TokenType type)
    {
        m_lastType = type;
        m_cursor = pos + 1;
        m_linePos++;
        Token t(type, m_linePos, m_line, Value{});
        return t;
    }

    Token EndToEndTokenizer::ParseStringLiteral(unsigned int& pos, MistakesContainer& mistakes)
    {
        char c = m_code[pos];
        unsigned int l = pos + 1;
        while (m_code.size() != l && m_code[l] != c && m_code[l] != '\n')
        {
            l++;
            if (m_code.size() == l || m_code[l] == '\n')
            {
                mistakes << CompilationMistake{
                    .Line = m_line, .Position = m_linePos, .Code = ErrorCode::UnfinishedString
                };
                l--;
                break;
            }
        }
        l -= pos;
        auto symbol = std::string(m_code.substr(pos + 1, l - 1));
        Token t(TokenType::LiteralString, m_linePos, m_line, symbol);
        m_cursor = pos + l + 1;
        m_linePos += l + 1;
        m_lastType = TokenType::LiteralString;
        return t;
    }

    Token EndToEndTokenizer::ParseComment(unsigned int& pos)
    {
        unsigned int l = pos + 2;
        while (!IsCommentEnd(m_code.substr(l)))
        {
            if (m_code[l] == '\n')
            {
                m_line++;
                m_linePos = 0;
            }
            m_linePos++;
            l++;
        }
        l -= pos;
        auto symbol = std::string(m_code.substr(pos + 2, l - 2));
        Token t(TokenType::Comment, m_linePos, m_line, symbol);
        m_cursor = pos + l + 2;
        m_linePos += l + 2;
        m_lastType = TokenType::Comment;
        return t;
    }

    Token EndToEndTokenizer::ParseOperator(unsigned int& pos)
    {
        char c = m_code[pos];
        m_lastType = TokenType::Operator;
        m_cursor = pos + 1;
        m_linePos++;
        std::string symbol(std::format("{}\0", c));
        Token t(TokenType::Operator, m_linePos, m_line, symbol);
        return t;
    }

    Token EndToEndTokenizer::ParseIdentifier(unsigned int& pos)
    {
        unsigned int l = pos + 1;
        while (m_code.size() > l && IsAllowedIdentifierChar(m_code[l]))
        {
            l++;
        }
        l -= pos;
        const auto symbol = std::string(m_code.substr(pos, l));
        if (IsBoolValue(symbol))
        {
            const bool val = symbol == "true";
            Token t(TokenType::LiteralBool, m_linePos, m_line, val);
            m_cursor = pos + l;
            m_linePos += l;
            m_lastType = TokenType::LiteralBool;
            return t;
        }
        Token t(TokenType::Identifier, m_linePos, m_line, symbol);
        m_cursor = pos + l;
        m_linePos += l;
        m_lastType = TokenType::Identifier;
        return t;
    }

    Token EndToEndTokenizer::ParseNumber(unsigned int& pos)
    {
        const std::string_view offset = m_code.substr(pos);
        double intPart;
        char* end;
        double fValue = strtod(offset.data(), &end);
        const auto endIndex = end - offset.data();
        m_linePos += endIndex;
        pos += endIndex;
        if (const double fractPart = modf(fValue, &intPart);
            fractPart == 0 || IsHexNumber(offset))
        {
            Token t(TokenType::LiteralNumber, m_linePos, m_line, static_cast<int64_t>(intPart));
            m_cursor = pos;
            m_lastType = TokenType::LiteralNumber;
            return t;
        }
        else
        {
            Token t(TokenType::LiteralFloatNumber, m_linePos, m_line, fValue);
            m_cursor = pos;
            m_lastType = TokenType::LiteralFloatNumber;
            return t;
        }
    }

    void EndToEndTokenizer::HandleUnknownToken(unsigned int& pos, MistakesContainer& mistakes)
    {
        mistakes << CompilationMistake
        {
            .Line = m_line, .Position = m_linePos, .Code = ErrorCode::UnknownToken, .Extra = &""[m_code[pos]]
        };
        pos++;
        m_cursor++;
        m_linePos++;
    }

    Token EndToEndTokenizer::GetNextToken()
    {
        auto t = std::move(m_tokens.front());
        m_tokens.pop_front();

        return t;
    }
}
