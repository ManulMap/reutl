#include "reutl/addr.hh"

#include <Windows.h>
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

TEST_CASE("memory protection methods")
{
    SECTION("is_accessible()")
    {
        REQUIRE_FALSE(reutl::make_addr(static_cast<void*>(nullptr)).is_accessible());
    }
    SECTION("is_readable()")
    {
        auto* const alloc =
            VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_READONLY);
        assert(alloc);

        REQUIRE(reutl::make_addr(alloc).is_readable());
        VirtualFree(alloc, sizeof(void*), MEM_RELEASE);
    }
    SECTION("is_writable()")
    {
        auto* const alloc =
            VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        assert(alloc);

        REQUIRE(reutl::make_addr(alloc).is_writable());
        VirtualFree(alloc, sizeof(void*), MEM_RELEASE);
    }
    SECTION("is_executable()")
    {
        auto* const alloc =
            VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE);
        assert(alloc);

        REQUIRE(reutl::make_addr(alloc).is_executable());
        VirtualFree(alloc, sizeof(void*), MEM_RELEASE);
    }
    SECTION("is_guarded()")
    {
        auto* const alloc = VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT,
                                         PAGE_READONLY | PAGE_GUARD);
        assert(alloc);

        REQUIRE(reutl::make_addr(alloc).is_guarded());
        VirtualFree(alloc, sizeof(void*), MEM_RELEASE);
    }
}