#pragma once
#include <any>
#include <functional>
#include <string>

#include <tss/variables/Variable.h>

namespace Trema
{
    namespace Style
    {
        struct OperatorData
        {
            std::string Name;
            int Priority;
            bool IsLeftAssociative;
            std::function<Value(const Value&, const Value&)> Operation;
        };

        class OperationsTable final
        {
        public:
            OperationsTable();
            OperationsTable(const OperationsTable& st) = delete;
            OperationsTable& operator=(const OperationsTable&) = delete;

            void InsertOperator(const std::string& name, int priority, bool leftAssociative, std::function<Value(const Value&, const Value&)> operation);
            const OperatorData& GetOperator(const std::string& name) const;
        private:
            std::unordered_map<std::string, OperatorData> m_operators;
        };
    }
}
