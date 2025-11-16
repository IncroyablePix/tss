#include <catch2/catch_test_macros.hpp>
#include <tss/utils/StringConversion.h>
#include <cstdint>
#include <string>

using namespace Trema::Utils;

TEST_CASE("ToHex padding and lowercase output for uint8_t")
{
    // Given
    constexpr uint8_t v = 0xAB; // 171

    // When
    const auto hex = ToHex(v);

    // Then
    REQUIRE(hex == "0xab");
    REQUIRE(hex.size() == 2 + 2); // 0x + 2 digits
}

TEST_CASE("ToHex zero value produces all zeros with correct width")
{
    // Given
    constexpr uint32_t v32 = 0u;
    constexpr uint16_t v16 = 0u;
    constexpr uint8_t v8 = 0u;

    // When
    const auto h32 = ToHex(v32);
    const auto h16 = ToHex(v16);
    const auto h8 = ToHex(v8);

    // Then
    REQUIRE(h32 == "0x00000000");
    REQUIRE(h16 == "0x0000");
    REQUIRE(h8 == "0x00");
}

TEST_CASE("ToHex produces correct width and value for uint16_t")
{
    // Given
    constexpr uint16_t v = 0x1; // should be padded

    // When
    const auto hex = ToHex(v);

    // Then
    REQUIRE(hex == "0x0001");
}

TEST_CASE("ToHex for uint32_t value DEADBEEF")
{
    // Given
    constexpr uint32_t v = 0xDEADBEEF;

    // When
    auto hex = ToHex(v);

    // Then
    REQUIRE(hex == "0xdeadbeef");
}

TEST_CASE("ToHex for uint64_t large value")
{
    // Given
    constexpr uint64_t v = 0x0123456789ABCDEFULL;

    // When
    auto hex = ToHex(v);

    // Then
    REQUIRE(hex == "0x0123456789abcdef");
    REQUIRE(hex.size() == 2 + 16); // 0x + 16 digits
}

TEST_CASE("ToHex for unsigned char produces two hex digits")
{
    // Given
    unsigned char c = 'A'; // 65 decimal = 0x41

    // When
    auto hex = ToHex(c);

    // Then
    REQUIRE(hex == "0x41");
}
