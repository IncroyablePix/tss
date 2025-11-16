#pragma once

#include <unordered_map>
#include <memory>
#include <sstream>
#include "Variable.h"

namespace Trema::Style
{
    class SymbolTable final : public std::enable_shared_from_this<SymbolTable>
    {
    public:
        SymbolTable();
        SymbolTable(const SymbolTable& st);
        SymbolTable& operator=(const SymbolTable&) = delete;

        template<typename T> void SetVariable(const std::string &name, const Value& value)
        {
            if(std::is_same_v<T, Float> ||
                std::is_same_v<T, Integer> ||
                std::is_same_v<T, char> ||
                std::is_same_v<T, bool>
                )
            {
                m_variables[name] = std::make_shared<Variable>(value);
            }
            else
            {
                throw std::runtime_error("Invalid variable type");
            }
        }

        bool HasVariable(const std::string &name) const { return m_variables.contains(name); }
        std::shared_ptr<Variable> GetVariable(const std::string& name) { return m_variables[name]; }
        void Append(const SymbolTable &st);

        friend std::ostream& operator<<(std::ostream& os, const SymbolTable& st);

        auto begin() { return m_variables.begin(); }
        auto end() { return m_variables.end(); }

    private:
        std::unordered_map<std::string, std::shared_ptr<Variable>> m_variables;
    };
}

