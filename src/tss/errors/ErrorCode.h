#pragma once

namespace Trema
{
    namespace Style
    {
        enum class ErrorCode : unsigned short
        {
        #pragma region Unknown
            UnknownError = 0,
        #pragma endregion

        #pragma region Tokenizer
            UnknownToken = 1001,
            UnfinishedString = 1002,
        #pragma endregion

        #pragma region Parser
            UndefinedSymbol = 2001,
            UnexpectedToken = 2002,
            TypeMismatch = 2003,
        #pragma endregion

        #pragma region Style
            ElementNotFound = 3001,
            MissingAttribute = 3002,
            MisplacedElement = 3003,
        #pragma endregion
        };
    }
}