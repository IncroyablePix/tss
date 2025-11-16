#include <catch2/catch_test_macros.hpp>
#include <tss/tokenization/Token.h>
#include <tss/tokenization/TokenType.h>
#include <tss/tokenization/TokenValue.h>
#include <sstream>
#include <cmath>

using namespace Trema::Style;

TEST_CASE("Token construction and getters work for all value types")
{
    // Given
    Token t1(TokenType::LiteralNumber, 42, 3, Integer{123});
    Token t2(TokenType::LiteralFloatNumber, 1, 2, Float{3.14});
    Token t3(TokenType::LiteralBool, 0, 0, true);
    Token t4(TokenType::LiteralString, 5, 6, std::string{"hello"});
    Token t5(TokenType::Comment, 7, 8, std::string{"comment"});
    Token t6(TokenType::EndOfCode, 0, 0, std::nullopt);

    // When

    // Then
    REQUIRE(t1.GetTokenType() == TokenType::LiteralNumber);
    REQUIRE(t1.GetPosition() == 42);
    REQUIRE(t1.GetLine() == 3);
    REQUIRE(std::get<Integer>(t1.GetValue()) == 123);
    REQUIRE(t2.GetTokenType() == TokenType::LiteralFloatNumber);
    REQUIRE(std::abs(std::get<Float>(t2.GetValue()) - 3.14) < 1e-6);
    REQUIRE(t3.GetTokenType() == TokenType::LiteralBool);
    REQUIRE(std::get<bool>(t3.GetValue()) == true);
    REQUIRE(t4.GetTokenType() == TokenType::LiteralString);
    REQUIRE(std::get<std::string>(t4.GetValue()) == "hello");
    REQUIRE(t5.GetTokenType() == TokenType::Comment);
    REQUIRE(std::get<std::string>(t5.GetValue()) == "comment");
    REQUIRE(t6.GetTokenType() == TokenType::EndOfCode);
    REQUIRE(std::holds_alternative<std::nullopt_t>(t6.GetValue()));
}

TEST_CASE("Token move constructor and move assignment")
{
    // Given
    Token t1(TokenType::Identifier, 10, 20, std::string{"id"});
    Token t3(TokenType::LiteralBool, 1, 2, false);

    // When
    Token t2(std::move(t1));
    t3 = std::move(t2);

    // Then
    REQUIRE(t3.GetTokenType() == TokenType::Identifier);
    REQUIRE(std::get<std::string>(t3.GetValue()) == "id");
}

TEST_CASE("Token::ValueAsString returns correct string for all value types")
{
    // Given
    Token t1(TokenType::LiteralNumber, 0, 0, Integer{42});
    Token t2(TokenType::LiteralFloatNumber, 0, 0, Float{2.5});
    Token t3(TokenType::LiteralBool, 0, 0, true);
    Token t4(TokenType::LiteralBool, 0, 0, false);
    Token t5(TokenType::LiteralString, 0, 0, std::string{"abc"});
    Token t6(TokenType::EndOfCode, 0, 0, std::nullopt);

    // When

    // Then
    REQUIRE(t1.ValueAsString() == "42");
    REQUIRE(t2.ValueAsString().substr(0,3) == "2.5");
    REQUIRE(t3.ValueAsString() == "true");
    REQUIRE(t4.ValueAsString() == "false");
    REQUIRE(t5.ValueAsString() == "abc");
    REQUIRE(t6.ValueAsString() == "null");
}

TEST_CASE("Token::GetIdentity produces expected output for key token types")
{
    // Given
    Token t1(TokenType::LiteralNumber, 1, 1, Integer{7});
    Token t2(TokenType::LiteralFloatNumber, 2, 2, Float{3.14});
    Token t3(TokenType::Identifier, 3, 3, std::string{"foo"});
    Token t4(TokenType::LiteralBool, 4, 4, true);
    Token t5(TokenType::LiteralString, 5, 5, std::string{"bar"});
    Token t6(TokenType::EndOfCode, 6, 6, std::nullopt);

    // When
    auto id1 = t1.GetIdentity();
    auto id2 = t2.GetIdentity();
    auto id3 = t3.GetIdentity();
    auto id4 = t4.GetIdentity();
    auto id5 = t5.GetIdentity();
    auto id6 = t6.GetIdentity();

    // Then
    REQUIRE(id1.find("Number") != std::string::npos);
    REQUIRE(id1.find("7") != std::string::npos);
    REQUIRE(id2.find("Float number") != std::string::npos);
    REQUIRE(id2.find("3.14") != std::string::npos);
    REQUIRE(id3.find("Identifier") != std::string::npos);
    REQUIRE(id3.find("foo") != std::string::npos);
    REQUIRE(id4.find("Bool") != std::string::npos);
    REQUIRE(id4.find("true") != std::string::npos);
    REQUIRE(id5.find("Literal String") != std::string::npos);
    REQUIRE(id5.find("bar") != std::string::npos);
    REQUIRE(id6.find("End of Code") != std::string::npos);
}

TEST_CASE("Token handles edge cases: empty string, nullopt, large numbers")
{
    // Given
    Token t1(TokenType::LiteralString, 0, 0, std::string{""});
    Token t2(TokenType::LiteralNumber, 0, 0, Integer{INT64_MAX});
    Token t3(TokenType::LiteralFloatNumber, 0, 0, Float{-1e10});
    Token t4(TokenType::EndOfCode, 0, 0, std::nullopt);

    // When

    // Then
    REQUIRE(t1.ValueAsString().empty());
    REQUIRE(t2.ValueAsString() == std::to_string(INT64_MAX));
    REQUIRE(t3.ValueAsString().find("-10000000000") == 0);
    REQUIRE(t4.ValueAsString() == "null");
}
