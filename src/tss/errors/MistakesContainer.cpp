#include <tss/errors/MistakesContainer.h>


    namespace Trema::Style
    {
        MistakesContainer &MistakesContainer::operator<<(CompilationMistake mistake)
        {
            push_back(std::move(mistake));
            return *this;
        }

        std::ostream &operator<<(std::ostream &os, const MistakesContainer &mistakes)
        {
            for(const auto& mistake : mistakes)
            {
                os << mistake;
            }
            return os;
        }
    }
