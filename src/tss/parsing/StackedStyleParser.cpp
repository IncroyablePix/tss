#include <tss/parsing/StackedStyleParser.h>
#include <fstream>
#include <tss/tokenization/EndToEndTokenizer.h>


namespace Trema::Style
{
    StackedStyleParser::StackedStyleParser(std::unique_ptr<ITokenizer> tokenizer, MistakesContainer& mistakes) :
        m_tokenizer(std::move(tokenizer)),
        m_pos(0),
        m_mistakes(mistakes)
    {
    }

    void StackedStyleParser::ParseFromCode(const std::string& code)
    {
        EndToEndTokenizer tokenizer(code, m_mistakes);

        if (tokenizer.Empty())
            return;

        std::stack<Token> tokens;

        auto currentSt = std::make_shared<SymbolTable>();
        m_symbolTables.push_back(currentSt);

        auto currentToken = tokenizer.GetNextToken();
        while (!tokenizer.Empty() && currentToken.GetTokenType() != TokenType::EndOfCode)
        {
            std::stack<Token> operators;
            switch (currentToken.GetTokenType())
            {
            case TokenType::Identity:
            case TokenType::Identifier:
            case TokenType::LiteralBool:
            case TokenType::LiteralString:
            case TokenType::PropertyAssignment:
            case TokenType::VariableAssignment:
            case TokenType::LiteralNumber:
            case TokenType::LiteralFloatNumber:
                tokens.push(std::move(currentToken));
                break;
            case TokenType::Operator:
                ProcessOperators(operators, currentToken, tokens);
                break;
            case TokenType::LeftCurlyBracket:
                currentSt = std::make_shared<SymbolTable>();
                m_symbolTables.push_back(currentSt);
                tokens.push(std::move(currentToken));
                break;
            case TokenType::RightCurlyBracket:
                AssignProps(tokens, currentSt);
                break;
            case TokenType::EndOfInstruction:
                if (!AssignVar(tokens, operators, currentSt))
                {
                    m_mistakes << CompilationMistake
                    {
                        .Line = currentToken.GetLine(),
                        .Position = currentToken.GetPosition(),
                        .Code = ErrorCode::UnexpectedToken,
                        .Extra = ";"
                    };
                }
                break;

            case TokenType::LeftParenthesis:
            case TokenType::RightParenthesis:
            case TokenType::EndOfCode:
            case TokenType::Comment:
                break;
            }

            currentToken = tokenizer.GetNextToken();
        }

        SaveTopSymbolTable("#");
    }

    void StackedStyleParser::ParseFromFile(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file)
            throw std::runtime_error(std::format("File not found: \"{}\"", path.string()));
        const std::string code{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

        file.close();
        ParseFromCode(code);
    }

    void StackedStyleParser::SetFromSymbolTables(const std::shared_ptr<SymbolTable>& symbolTable,
                                                 const std::string& propName, const std::string& varName) const
    {
        bool found = false;
        for (const auto& st : m_symbolTables)
        {
            if (!st->HasVariable(varName))
            {
                continue;
            }

            found = true;

            const auto v = st->GetVariable(varName);
            if (std::holds_alternative<Integer>(v->GetValue()))
                symbolTable->SetVariable<Integer>(propName, v->CopyValue());
            else if (std::holds_alternative<Float>(v->GetValue()))
                symbolTable->SetVariable<Float>(propName, v->CopyValue());
            else if (std::holds_alternative<std::string>(v->GetValue()))
                symbolTable->SetVariable<char>(propName, v->CopyValue());
            else if (std::holds_alternative<bool>(v->GetValue()))
                symbolTable->SetVariable<bool>(propName, v->CopyValue());
        }

        if (!found)
        {
            m_mistakes << CompilationMistake
            {
                .Line = 1, .Position = 0, .Code = ErrorCode::UndefinedSymbol, .Extra = std::string(varName)
            };
        }
    }

    std::optional<Value> StackedStyleParser::GetNextTokenValue(std::stack<Token>& tokens) const
    {
        const auto token = std::move(tokens.top());
        tokens.pop();

        if (token.GetTokenType() == TokenType::Identifier)
        {
            const auto variableName = std::get<std::string>(token.GetValue());
            for (const auto& st : m_symbolTables)
            {
                if (st->HasVariable(variableName))
                {
                    auto v = st->GetVariable(variableName)->GetValue();
                    if (std::holds_alternative<Integer>(v))
                        return v;
                    if (std::holds_alternative<Float>(v))
                        return v;

                    m_mistakes << CompilationMistake
                    {
                        .Line = token.GetLine(), .Position = token.GetPosition(),
                        .Code = ErrorCode::TypeMismatch, .Extra = std::string(variableName)
                    };
                    return {};
                }
            }
        }
        else if (token.GetTokenType() == TokenType::LiteralNumber || token.GetTokenType() ==
            TokenType::LiteralFloatNumber)
        {
            return token.GetValue();
        }
        else
        {
            // TODO: process error
            m_mistakes << CompilationMistake
            {
                .Line = token.GetLine(), .Position = token.GetPosition(), .Code = ErrorCode::UnexpectedToken,
                .Extra = token.GetIdentity()
            };
        }

        return {};
    }

    bool StackedStyleParser::ProcessOperators(std::stack<Token>& operators,
                                              Token& currentOperator,
                                              std::stack<Token>& tokens) const
    {
        while (!operators.empty())
        {
            const auto& operator2Token = operators.top();
            const auto& op1 = m_operationsTable.GetOperator(std::get<std::string>(currentOperator.GetValue()));
            const auto& op2 = m_operationsTable.GetOperator(std::get<std::string>(operator2Token.GetValue()));

            if (op2.Priority > op1.Priority || (op2.Priority == op1.Priority && op2.IsLeftAssociative))
            {
                operators.pop();

                const auto value1 = GetNextTokenValue(tokens);
                const auto value2 = GetNextTokenValue(tokens);

                if (!value1.has_value() || !value2.has_value())
                {
                    m_mistakes << CompilationMistake
                    {
                        .Line = currentOperator.GetLine(), .Position = currentOperator.GetPosition(),
                        .Code = ErrorCode::UnexpectedToken,
                        .Extra = std::format("Missing value around {}", currentOperator.GetIdentity())
                    };
                    return false;
                }

                const auto result = op2.Operation(*value2, *value1);

                if (std::holds_alternative<Float>(result))
                {
                    Token t(TokenType::LiteralFloatNumber, currentOperator.GetPosition(),
                            currentOperator.GetLine(), result);
                    tokens.push(std::move(t));
                }
                else if (std::holds_alternative<Integer>(result))
                {
                    Token t(TokenType::LiteralNumber, currentOperator.GetPosition(),
                            currentOperator.GetLine(), result);
                    tokens.push(std::move(t));
                }
            }
            else
            {
                break;
            }
        }

        operators.push(std::move(currentOperator));
        return true;
    }

    bool StackedStyleParser::AssignVar(std::stack<Token>& tokens,
                                       std::stack<Token>& operators,
                                       const std::shared_ptr<SymbolTable>& currentSt) const
    {
        // Shunting Yard
        while (!operators.empty())
        {
            const auto operatorToken = std::move(operators.top());
            operators.pop();

            const auto value1 = GetNextTokenValue(tokens);
            const auto value2 = GetNextTokenValue(tokens);

            const auto& op2 = m_operationsTable.GetOperator(std::get<std::string>(operatorToken.GetValue()));

            if (!value1.has_value() || !value2.has_value())
            {
                m_mistakes << CompilationMistake
                {
                    .Line = operatorToken.GetLine(), .Position = operatorToken.GetPosition(),
                    .Code = ErrorCode::UnexpectedToken,
                    .Extra = std::format("Missing value around {}", operatorToken.GetIdentity())
                };
                return false;
            }

            const auto result = op2.Operation(*value2, *value1);

            if (std::holds_alternative<Float>(result))
            {
                Token t(TokenType::LiteralFloatNumber, operatorToken.GetPosition(),
                        operatorToken.GetLine(), result);
                tokens.push(std::move(t));
            }
            else if (std::holds_alternative<Integer>(result))
            {
                Token t(TokenType::LiteralNumber, operatorToken.GetPosition(),
                        operatorToken.GetLine(), result);
                tokens.push(std::move(t));
            }
        }

        //
        if (tokens.size() < 3)
            return false;

        // Assign to variable
        auto val = std::move(tokens.top());
        tokens.pop();
        auto assigner = std::move(tokens.top());
        tokens.pop();
        auto propName = std::move(tokens.top());
        tokens.pop();

        if (propName.GetTokenType() == TokenType::Identifier &&
            (
                val.GetTokenType() == TokenType::Identifier ||
                val.GetTokenType() == TokenType::LiteralBool ||
                val.GetTokenType() == TokenType::LiteralNumber ||
                val.GetTokenType() == TokenType::LiteralFloatNumber ||
                val.GetTokenType() == TokenType::LiteralString
            ) &&
            (
                assigner.GetTokenType() == TokenType::VariableAssignment ||
                assigner.GetTokenType() == TokenType::PropertyAssignment)
        )
        {
            if (val.GetTokenType() == TokenType::LiteralBool)
                currentSt->SetVariable<bool>(std::get<std::string>(propName.GetValue()), val.GetValue());
            else if (val.GetTokenType() == TokenType::LiteralFloatNumber)
                currentSt->SetVariable<Float>(std::get<std::string>(propName.GetValue()), val.GetValue());
            else if (val.GetTokenType() == TokenType::LiteralNumber)
                currentSt->SetVariable<Integer>(std::get<std::string>(propName.GetValue()), val.GetValue());
            else if (val.GetTokenType() == TokenType::LiteralString)
                currentSt->SetVariable<std::string>(std::get<std::string>(propName.GetValue()), val.GetValue());
            else if (val.GetTokenType() == TokenType::Identifier)
                SetFromSymbolTables(currentSt, std::get<std::string>(propName.GetValue()),
                                    std::get<std::string>(val.GetValue()));

            return true;
        }
        return false;
    }

    void StackedStyleParser::AssignProps(std::stack<Token>& tokens,
                                         std::shared_ptr<SymbolTable>& currentSt)
    {
        if (tokens.size() < 2)
            throw std::runtime_error(R"(Unexpected symbol "}")");

        tokens.pop(); // remove '{'

        // Getting identifier
        const auto topToken = std::move(tokens.top());
        tokens.pop();

        std::stringstream ss;
        if (topToken.GetTokenType() == TokenType::Identity)
        {
            ss << "#";
        }
        else if (topToken.GetTokenType() == TokenType::Identifier)
        {
            if (!tokens.empty() && tokens.top().GetTokenType() == TokenType::Identity)
            {
                ss << "#";
                tokens.pop();
            }
            ss << std::get<std::string>(topToken.GetValue());
        }

        SaveTopSymbolTable(ss.str());

        currentSt = m_symbolTables.back();
    }

    void StackedStyleParser::SaveTopSymbolTable(std::string name)
    {
        const auto topSymbolTable = m_symbolTables.back();
        if (m_variables.contains(name))
        {
            m_variables[std::move(name)]->Append(*topSymbolTable);
        }
        else
        {
            m_variables[std::move(name)] = topSymbolTable;
        }
        m_symbolTables.pop_back();
    }
}
