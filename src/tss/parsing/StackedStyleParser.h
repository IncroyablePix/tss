#pragma once
#include <stack>
#include <memory>
#include <filesystem>
#include <tss/parsing/StyleParser.h>
#include <tss/errors/MistakesContainer.h>
#include <tss/tokenization/Token.h>
#include <tss/variables/OperationsTable.h>
#include <tss/tokenization/ITokenizer.h>

namespace Trema
{
    namespace Style
    {
        class StackedStyleParser final : public StyleParser
        {
        public:
            StackedStyleParser(std::unique_ptr<ITokenizer> tokenizer, MistakesContainer& mistakes);
            StackedStyleParser(const StackedStyleParser&) = delete;
            StackedStyleParser& operator=(const StackedStyleParser&) = delete;
            ~StackedStyleParser() override = default;
            void ParseFromCode(const std::string &code) override;
            void ParseFromFile(const std::filesystem::path &path) override;

        private:
            std::unique_ptr<ITokenizer> m_tokenizer;
            unsigned int m_pos;
            OperationsTable m_operationsTable;
            MistakesContainer& m_mistakes;

            void SetFromSymbolTables(const std::shared_ptr<SymbolTable>& st, const std::string& propName, const std::string& varName) const;
            bool ProcessOperators(std::stack<Token>& operators, Token& currentOperator, std::stack<Token>& tokens) const;
            bool AssignVar(std::stack<Token>& tokens, std::stack<Token>& operators, const std::shared_ptr<SymbolTable>& currentSt) const;
            void AssignProps(std::stack<Token>& tokens, std::shared_ptr<SymbolTable>& currentSt);
            void SaveTopSymbolTable(std::string name);

            std::optional<Value> GetNextTokenValue(std::stack<Token> &tokens) const;
        };
    }
}
