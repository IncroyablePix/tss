#include <catch2/catch_test_macros.hpp>
#include <tss/errors/CompilationMistake.h>
#include <tss/errors/MistakesContainer.h>
#include <sstream>

using namespace Trema::Style;

TEST_CASE("MistakesContainer basic operations and ostream output")
{
    // Given
    MistakesContainer container;
    const CompilationMistake m1{1, 2, false, ErrorCode::UnknownToken, "foo"};
    const CompilationMistake m2{3, 4, true, ErrorCode::UnfinishedString, "bar"};

    // When
    container << m1 << m2;

    // Then
    REQUIRE(container.size() == 2);
    REQUIRE(container[0].Line == 1);
    REQUIRE(container[1].Fatal == true);

    // When
    std::ostringstream oss;
    oss << container;

    // Then
    auto out = oss.str();
    REQUIRE(out.find("Unknown token") != std::string::npos);
    REQUIRE(out.find("Unfinished string") != std::string::npos);
    REQUIRE(out.find("foo") != std::string::npos);
    REQUIRE(out.find("bar") != std::string::npos);
}

TEST_CASE("MistakesContainer handles empty and clear correctly")
{
    // Given
    MistakesContainer container;

    // Then
    REQUIRE(container.empty());

    // When
    const CompilationMistake m{1, 1, false, ErrorCode::UnknownError, ""};
    container << m;
    REQUIRE_FALSE(container.empty());
    container.clear();
    REQUIRE(container.empty());
}
