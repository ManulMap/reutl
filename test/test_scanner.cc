#include "reutl/scanner.hh"

#include <catch2/catch_test_macros.hpp>
#include <array>

const std::array<std::uint8_t, 75> k_shellcode = {
    /* 0 */ 0xDD,  0x1A, 0x6F, 0x61, 0x11, 0x66, // begin pattern
    /* 6 */ 0x32,  0xC0, 0x72, 0x66, 0x87, 0x15, 0x9A, 0x12, 0x7F, 0x12,
    /* 16 */ 0x87, 0x50, 0xB0, 0xA2, 0xE6, 0x81, 0x52, 0x55, 0x12, 0x40,
    /* 26 */ 0x28, 0x27, 0x26, 0x78, 0xB7, 0xFA, 0xF2, 0x8F, 0x6A, 0x4F,
    /* 36 */ 0x68, 0x2F, 0x73, 0xFA, // mid pattern
    /* 40 */ 0x23, 0xD3, 0xD2, 0xBD, 0xDB, 0xFF, 0xFF, 0x12, 0xD1, 0xA2,
    /* 50 */ 0x28, 0x27, 0x26, 0x78, 0xB7, 0xFA, 0xF2, 0x8F, 0x6A, 0x4F,
    /* 60 */ 0x87, 0x50, 0xB0, 0xA1, 0xE5, 0x21, 0x32, 0x55, 0x12, 0x40,
    /* 70 */ 0xF1, 0x06, 0x61, 0x7B}; // end pattern

using reutl::find_pattern;

TEST_CASE("find_pattern()")
{
    SECTION("mid pattern")
    {
        const auto res = reutl::find_pattern<"68 ?? ?? fa">(k_shellcode.data(), k_shellcode.size());

        const auto* const mid = k_shellcode.data() + 36;

        REQUIRE(res.has_value());
        REQUIRE(res.value() == mid);
    }
    SECTION("start pattern")
    {
        const auto res = find_pattern<"dd 1a 6f ?? ?? 66">(k_shellcode.data(), k_shellcode.size());
        REQUIRE(res.has_value());

        REQUIRE(res.value() == k_shellcode.data());
    }
    SECTION("end pattern")
    {
        auto res = reutl::find_pattern<"f1 06 ?? 7b">(k_shellcode.data(), k_shellcode.size());
        REQUIRE(res.has_value());

        const auto* const end = k_shellcode.data() + 70;

        REQUIRE(res.value() == end);
    }
    SECTION("non-existent pattern")
    {
        auto res = reutl::find_pattern<"ff aa 33 22 99 77 44">(k_shellcode.data(), k_shellcode.size());
        REQUIRE_FALSE(res.has_value());
    }
}