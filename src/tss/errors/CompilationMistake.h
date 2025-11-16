#pragma once

#include <ostream>
#include <string>
#include <tss/errors/ErrorCode.h>

namespace Trema
{
    namespace Style
    {
        struct CompilationMistake final
        {
            unsigned int Line { 0 };
            unsigned int Position { 0 };
            bool Fatal { false };
            ErrorCode Code {ErrorCode::UnknownError };
            std::string Extra;

            friend std::ostream& operator<<(std::ostream& os, const CompilationMistake& st);
        };
    }
}