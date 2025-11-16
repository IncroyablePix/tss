#include <catch2/catch_test_macros.hpp>
#include <tss/errors/CompilationMistake.h>
#include <tss/errors/MistakesContainer.h>
#include <sstream>

using namespace Trema::Style;

TEST_CASE("CompilationMistake default construction and field assignment")
{
    // Given
    CompilationMistake m;

    // When
    m.Line = 42;
    m.Position = 7;
    m.Fatal = true;
    m.Code = ErrorCode::UnfinishedString;
    m.Extra = "Unclosed quote";

    // Then
    REQUIRE(m.Line == 42);
    REQUIRE(m.Position == 7);
    REQUIRE(m.Fatal == true);
    REQUIRE(m.Code == ErrorCode::UnfinishedString);
    REQUIRE(m.Extra == "Unclosed quote");
}

TEST_CASE("CompilationMistake ostream operator<< produces correct string") {
    // Given
    const CompilationMistake m{10, 5, false, ErrorCode::UnknownToken, "@"};
    std::ostringstream oss;

    // When
    oss << m;

    // Then
    auto str = oss.str();
    REQUIRE(str.find("Unknown token") != std::string::npos);
    REQUIRE(str.find("10:5") != std::string::npos);
    REQUIRE(str.find('@') != std::string::npos);
}
