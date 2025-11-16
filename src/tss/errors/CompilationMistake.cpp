#include <tss/errors/CompilationMistake.h>

namespace Trema
{
    namespace Style
    {
        std::ostream &operator<<(std::ostream &os, const CompilationMistake &m)
        {
            switch(m.Code)
            {
            case ErrorCode::UnknownError:
                os << "Unknown error (" << static_cast<unsigned short>(m.Code)  << " | " << m.Line << ":" << m.Position << ")";
                break;

            case ErrorCode::UnknownToken:
                os << "Unknown token (" << static_cast<unsigned short>(m.Code)  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;
            case ErrorCode::UnfinishedString:
                os << "Unfinished string (" << (unsigned short)m.Code  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;

            case ErrorCode::UndefinedSymbol:
                os << "Undefined symbol (" << static_cast<unsigned short>(m.Code)  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;
            case ErrorCode::UnexpectedToken:
                os << "Unexpected token (" << (unsigned short)m.Code  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;

            case ErrorCode::ElementNotFound:
                os << "Element not found (" << static_cast<unsigned short>(m.Code)  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;
            case ErrorCode::MissingAttribute:
                os << "Missing attribute (" << static_cast<unsigned short>(m.Code)  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;
            case ErrorCode::MisplacedElement:
                os << "Misplaced element (" << static_cast<unsigned short>(m.Code)  << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;
            case ErrorCode::TypeMismatch:
                os << "Type mismatch (" << static_cast<unsigned short>(m.Code) << " | " << m.Line << ":" << m.Position << "): " << m.Extra;
                break;
            }

            return os << "\n";
        }
    }
}