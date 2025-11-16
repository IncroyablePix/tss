#pragma once

namespace Trema
{
    namespace Style
    {
        enum class TokenType
        {
            LiteralFloatNumber = 1, // number
            Identifier = 2, // property name, applier, variable
            Identity = 3, // #
            EndOfInstruction = 4, // ;
            LeftParenthesis = 5, // (
            RightParenthesis = 6, // )
            LeftCurlyBracket = 7, // {
            RightCurlyBracket = 8, // }
            PropertyAssignment = 9, // :
            VariableAssignment = 10, // =
            LiteralString = 11, // " " || ' '
            LiteralNumber = 12, // number
            LiteralBool = 13,
            Comment = 14, // /* */
            Operator = 15, // + - * / %
            EndOfCode = -1 // End of expression
        };
    }
}