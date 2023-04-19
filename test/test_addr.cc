#include "reutl/addr.hh"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <array>

struct TestStruct {
    std::int32_t beg_val = 24;

    [[maybe_unused]] std::array<std::byte, 10> pad = {};

    std::int32_t end_value = 42;
};

TEST_CASE("to_ptr()")
{
    const auto test_struct = std::make_unique<TestStruct>();

    const auto* const ptr = reutl::make_addr(test_struct.get()) //
                                .to_ptr<TestStruct>();

    REQUIRE(ptr->end_value == 42);
    REQUIRE(ptr->beg_val == 24);
}