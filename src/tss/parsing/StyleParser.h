#pragma once
#include <deque>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <tss/variables/SymbolTable.h>

namespace Trema
{
    namespace Style
    {
        class StyleParser
        {
        public:
            virtual ~StyleParser() = default;
            virtual void ParseFromFile(const std::filesystem::path &path) = 0;
            virtual void ParseFromCode(const std::string& code) = 0;

            void ClearVariables() { m_variables.clear(); }
            [[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<SymbolTable>>& GetVariables() { return m_variables; };

        protected:
            std::unordered_map<std::string, std::shared_ptr<SymbolTable>> m_variables;
            std::deque<std::shared_ptr<SymbolTable>> m_symbolTables;
        };
    }
}
