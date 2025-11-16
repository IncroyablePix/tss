#include <tss/parsing/StackedStyleParser.h>
#include <tss/tokenization/EndToEndTokenizer.h>
#include <tss/variables/SymbolTable.h>
#include <tss/errors/MistakesContainer.h>
#include <catch2/catch_test_macros.hpp>

using namespace Trema::Style;

TEST_CASE("ParseFromCode_EmptyCode", "[StackedStyleParser]")
{
    // Given
    MistakesContainer mistakes;
    auto tokenizer = std::make_unique<EndToEndTokenizer>("", mistakes);
    StackedStyleParser parser(std::move(tokenizer), mistakes);

    // When
    parser.ParseFromCode("");

    // Then
    REQUIRE(mistakes.empty());
}

TEST_CASE("ParseFromCode_SimpleAssignment", "[StackedStyleParser]")
{
    // Given
    MistakesContainer mistakes;
    std::string code = "red: 5;";
    auto tokenizer = std::make_unique<EndToEndTokenizer>(code, mistakes);
    StackedStyleParser parser(std::move(tokenizer), mistakes);

    // When
    parser.ParseFromCode(code);

    // Then
    const auto symbolTable = parser.GetVariables().begin()->second;
    REQUIRE(symbolTable->HasVariable("red"));
    REQUIRE(std::get<Integer>(symbolTable->GetVariable("red")->GetValue()) == 5);
}

TEST_CASE("ParseFromFile_FileNotFound", "[StackedStyleParser]")
{
    // Given
    MistakesContainer mistakes;
    auto tokenizer = std::make_unique<EndToEndTokenizer>("", mistakes);
    StackedStyleParser parser(std::move(tokenizer), mistakes);

    // When

    // Then
    REQUIRE_THROWS_AS(parser.ParseFromFile("nonexistent_file.tss"), std::runtime_error);
}

TEST_CASE("ParseFromCode_InvalidToken", "[StackedStyleParser]")
{
    // Given
    MistakesContainer mistakes;
    std::string code = "x = ;";
    auto tokenizer = std::make_unique<EndToEndTokenizer>(code, mistakes);
    StackedStyleParser parser(std::move(tokenizer), mistakes);

    // When
    parser.ParseFromCode(code);

    // Then
    REQUIRE_FALSE(mistakes.empty());
    REQUIRE(mistakes.front().Code == ErrorCode::UnexpectedToken);
}

TEST_CASE("In the right symbol table", "[StackedStyleParser]")
{
    // Given
    MistakesContainer mistakes;
    const std::string code = "#someElement {"
                       "  width: 100;"
                       "  height: 200;"
                       "}";
    auto tokenizer = std::make_unique<EndToEndTokenizer>(code, mistakes);
    StackedStyleParser parser(std::move(tokenizer), mistakes);

    // When
    parser.ParseFromCode(code);

    // Then
    REQUIRE(mistakes.empty());
    const auto symbolTable = parser.GetVariables().at("#someElement");
    REQUIRE(symbolTable->HasVariable("width"));
    REQUIRE(symbolTable->HasVariable("height"));
    REQUIRE(std::get<Integer>(symbolTable->GetVariable("width")->GetValue()) == 100);
    REQUIRE(std::get<Integer>(symbolTable->GetVariable("height")->GetValue()) == 200);
}

TEST_CASE("Variable copying", "[StackedStyleParser]")
{
    // Given
    MistakesContainer mistakes;
    const std::string code = "#element {\n"
                       "  baseWidth: 150;\n"
                       "  width: baseWidth;\n"
                       "}\n";
    auto tokenizer = std::make_unique<EndToEndTokenizer>(code, mistakes);
    StackedStyleParser parser(std::move(tokenizer), mistakes);

    // When
    parser.ParseFromCode(code);

    // Then
    REQUIRE(mistakes.empty());
    const auto symbolTable = parser.GetVariables().at("#element");
    REQUIRE(symbolTable->HasVariable("baseWidth"));
    REQUIRE(symbolTable->HasVariable("width"));
    REQUIRE(std::get<Integer>(symbolTable->GetVariable("baseWidth")->GetValue()) == 150);
    REQUIRE(std::get<Integer>(symbolTable->GetVariable("width")->GetValue()) == 150);
}