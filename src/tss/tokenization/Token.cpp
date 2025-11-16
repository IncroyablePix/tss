#include <tss/tokenization/Token.h>
#include <sstream>
#include <utility>
#include <tss/utils/StringConversion.h>

namespace Trema::Style
{
    Token::Token(const TokenType tokenType, unsigned int position, unsigned int line, Value value)
        : m_tokenType(tokenType), m_value(std::move(value)), m_position(position), m_line(line)
    {
    }

    Token::Token(Token&& other) noexcept
        : m_tokenType(other.m_tokenType), m_value(std::move(other.m_value)), m_position(other.m_position), m_line(other.m_line)
    {

    }

    Token& Token::operator=(Token&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        m_tokenType = other.m_tokenType;
        m_value = std::move(other.m_value);
        m_position = other.m_position;
        m_line = other.m_line;
        return *this;
    }

    Token::~Token()
    {
    }

    std::string Token::GetIdentity() const
    {
        std::stringstream ss;
        ss << "<";
        switch(m_tokenType)
        {
        case TokenType::LiteralNumber:
            ss << "Number ('" << ValueAsString() << " - " << Trema::Utils::ToHex(ValueAsString()) << "'):";
            break;
        case TokenType::LiteralFloatNumber:
            ss << "Float number ('" << ValueAsString() << "'):";
            break;
        case TokenType::Identifier:
            ss << "Identifier ('" << ValueAsString() << "'):";
            break;
        case TokenType::Identity:
            ss << "Identity ('#'):";
            break;
        case TokenType::EndOfInstruction:
            ss << "End of instruction (';'):";
            break;
        case TokenType::LeftParenthesis:
            ss << "Left Parenthesis ('('):";
            break;
        case TokenType::RightParenthesis:
            ss << "Right Parenthesis (')'):";
            break;
        case TokenType::LeftCurlyBracket:
            ss << "Left Curly ('{'):";
            break;
        case TokenType::RightCurlyBracket:
            ss << "Right Curly ('}'):";
            break;
        case TokenType::PropertyAssignment:
            ss << "Property Assignment (':'):";
            break;
        case TokenType::VariableAssignment:
            ss << "Variable Assignement ('='):";
            break;
        case TokenType::LiteralString:
            ss << "Literal String ('" << ValueAsString() << "'):";
            break;
        case TokenType::LiteralBool:
            ss << "Literal Bool ('" << ValueAsString() << "'):";
            break;
        case TokenType::Comment:
            ss << "Comment ('" << ValueAsString() << "'):";
            break;
        case TokenType::EndOfCode:
            ss << "End of Code:";
            break;
        case TokenType::Operator:
            ss << "Operator ('" << ValueAsString() << "'):";
            break;
        }
        ss << GetPosition() << ">\n";
        auto str = ss.str();

        return str;
    }

    std::string Token::ValueAsString() const
    {
        return Trema::Style::GetIdentity(m_value);
    }
}
