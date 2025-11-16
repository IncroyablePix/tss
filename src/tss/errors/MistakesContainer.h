#pragma once
#include <deque>
#include <tss/errors/CompilationMistake.h>

namespace Trema
{
    namespace Style
    {
        class MistakesContainer final : public std::deque<CompilationMistake>
        {
        public:
            MistakesContainer& operator<<(CompilationMistake mistake);
            friend std::ostream& operator<<(std::ostream& os, const MistakesContainer& mistakes);
        };
    }
}
