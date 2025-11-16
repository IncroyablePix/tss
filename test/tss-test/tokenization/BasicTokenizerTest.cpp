#include <tss/tokenization/EndToEndTokenizer.h>
#include <catch2/catch_test_macros.hpp>
#include <sstream>

using namespace Trema::Style;

TEST_CASE("Getting the good amount of tokens")
{
    // Given
    const std::string code = "#element { prop: variable; }";
    MistakesContainer mistakes;
    int count = 0;

    // When
    EndToEndTokenizer t(code, mistakes);
    while (!t.Empty())
    {
        auto token = t.GetNextToken();
        count++;
    }

    // Then
    REQUIRE(count == 9);
}

TEST_CASE("Last token is T_STOP")
{
    // Given
    const std::string code = "#element { prop: variable; }";
    MistakesContainer mistakes;
    std::unique_ptr<Token> token;
    TokenType lastType;

    // When
    EndToEndTokenizer t(code, mistakes);
    while (!t.Empty())
    {
        lastType = t.GetNextToken().GetTokenType();
    }

    // Then
    REQUIRE(lastType == TokenType::EndOfCode);
}

TEST_CASE("Identifies literal string")
{
    // Given
    const std::string code = R"("Hello, I'm testing out my code")";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::LiteralString);
    REQUIRE(std::get<std::string>(token.GetValue()) == "Hello, I'm testing out my code");
}

TEST_CASE("Identifies boolean value")
{
    // Given
    const std::string code = "false";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::LiteralBool);
    REQUIRE(std::get<bool>(token.GetValue()) == false);
}

TEST_CASE("Identifies number as float")
{
    // Given
    const std::string code = "1.5";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::LiteralFloatNumber);
    REQUIRE(std::get<double>(token.GetValue()) == 1.5);
}

TEST_CASE("Identifies number as integer")
{
    // Given
    const std::string code = "34";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    auto token = std::move(t.GetNextToken());

    // Then
    REQUIRE(token.GetTokenType() == TokenType::LiteralNumber);
    REQUIRE(std::get<int64_t>(token.GetValue()) == 34);
}

TEST_CASE("Has error for unfinished string")
{
    // Given
    const std::string code = R"("Hello, I'm testing out my code\n)";
    MistakesContainer mistakes;
    std::unique_ptr<Token> token;

    // When
    EndToEndTokenizer t(code, mistakes);

    // Then
    REQUIRE(mistakes.size() == 1);
    REQUIRE(mistakes[0].Code == ErrorCode::UnfinishedString);
}

TEST_CASE("Ignores comments")
{
    // Given
    const std::string code = R"(#test { property: /* This is a comment */ "value"; /* This is also a comment */ })";
    MistakesContainer mistakes;
    std::unique_ptr<Token> token;

    // When
    const EndToEndTokenizer t(code, mistakes);

    // Then
    REQUIRE(mistakes.empty());
    REQUIRE(t.Size() == 11);
}

TEST_CASE("Catches comment")
{
    // Given
    std::string comment = "Testing out my stuff";
    MistakesContainer mistakes;
    std::stringstream ss;
    ss << "/*" << comment << "*/";
    const std::string code { ss.str() };

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::Comment);
    REQUIRE(std::get<std::string>(token.GetValue()) == comment);
}

TEST_CASE("Handles empty input")
{
    // Given
    const std::string code;
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    const Token token = t.GetNextToken();

    // Then
    REQUIRE(t.Empty());
    REQUIRE(token.GetTokenType() == TokenType::EndOfCode);
}

TEST_CASE("Handles whitespace-only input")
{
    // Given
    const std::string code = "   \t\n  ";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    const Token token = t.GetNextToken();

    // Then
    REQUIRE(t.Empty());
    REQUIRE(token.GetTokenType() == TokenType::EndOfCode);
}

TEST_CASE("Identifies all single-character tokens")
{
    // Given
    const std::string code = "#;(){}:=+";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);

    // Then
    const std::vector expected =
    {
        TokenType::Identity,             // #
        TokenType::EndOfInstruction,     // ;
        TokenType::LeftParenthesis,      // (
        TokenType::RightParenthesis,     // )
        TokenType::LeftCurlyBracket,     // {
        TokenType::RightCurlyBracket,    // }
        TokenType::PropertyAssignment,   // :
        TokenType::VariableAssignment,   // =
        TokenType::Operator,             // +
        TokenType::EndOfCode
    };

    for (const auto type : expected)
    {
        REQUIRE(!t.Empty());
        REQUIRE(t.GetNextToken().GetTokenType() == type);
    }
}

TEST_CASE("Identifies identifiers and numbers")
{
    // Given
    const std::string code = "foo 123 4.56";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);

    // Then
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::Identifier);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::LiteralNumber);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::LiteralFloatNumber);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::EndOfCode);
}

TEST_CASE("Identifies string, bool, and comment")
{
    // Given
    const std::string code = "\"str\" true /* comment */";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);

    // Then
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::LiteralString);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::LiteralBool);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::Comment);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::EndOfCode);
}

TEST_CASE("Handles unterminated string error")
{
    // Given
    const std::string code = "\"unterminated string";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::LiteralString);
    REQUIRE(!mistakes.empty());
}

TEST_CASE("Handles long identifier")
{
    // Given
    std::string longIdent(1024, 'a');
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(longIdent, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::Identifier);
    REQUIRE(std::get<std::string>(token.GetValue()) == longIdent);
}

TEST_CASE("Handles mixed token sequence")
{
    // Given
    const std::string code = "#id { key: 123; flag: true; str: \"abc\"; }";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);

    // Then
    const std::vector expected =
    {
        TokenType::Identity, TokenType::Identifier, TokenType::LeftCurlyBracket,
        TokenType::Identifier, TokenType::PropertyAssignment, TokenType::LiteralNumber, TokenType::EndOfInstruction,
        TokenType::Identifier, TokenType::PropertyAssignment, TokenType::LiteralBool, TokenType::EndOfInstruction,
        TokenType::Identifier, TokenType::PropertyAssignment, TokenType::LiteralString, TokenType::EndOfInstruction,
        TokenType::RightCurlyBracket, TokenType::EndOfCode
    };
    for (auto type : expected) {
        REQUIRE(!t.Empty());
        REQUIRE(t.GetNextToken().GetTokenType() == type);
    }
}

TEST_CASE("Handles UTF-8 emojis in string literals")
{
    // Given
    const std::string code = "\"Hello 😀 World 🎉\"";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::LiteralString);
    REQUIRE(std::get<std::string>(token.GetValue()) == "Hello 😀 World 🎉");
}

TEST_CASE("Handles UTF-8 characters in identifiers")
{
    // Given
    const std::string code = "café";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::Identifier);
    REQUIRE(std::get<std::string>(token.GetValue()) == "café");
}

TEST_CASE("Handles UTF-8 emojis in identifiers")
{
    // Given
    const std::string code = "variable😀";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::Identifier);
    REQUIRE(std::get<std::string>(token.GetValue()) == "variable😀");
}

TEST_CASE("Handles Chinese characters in identifiers")
{
    // Given
    const std::string code = "变量名称";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);
    Token token = t.GetNextToken();

    // Then
    REQUIRE(token.GetTokenType() == TokenType::Identifier);
    REQUIRE(std::get<std::string>(token.GetValue()) == "变量名称");
}

TEST_CASE("Handles mixed ASCII and UTF-8 in code")
{
    // Given
    const std::string code = "#emoji { prop: \"🚀 test\"; }";
    MistakesContainer mistakes;

    // When
    EndToEndTokenizer t(code, mistakes);

    // Then
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::Identity);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::Identifier);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::LeftCurlyBracket);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::Identifier);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::PropertyAssignment);

    Token stringToken = t.GetNextToken();
    REQUIRE(stringToken.GetTokenType() == TokenType::LiteralString);
    REQUIRE(std::get<std::string>(stringToken.GetValue()) == "🚀 test");

    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::EndOfInstruction);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::RightCurlyBracket);
    REQUIRE(t.GetNextToken().GetTokenType() == TokenType::EndOfCode);
}

