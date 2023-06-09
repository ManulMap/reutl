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

    const auto* const ptr = reutl::Addr(test_struct.get()) //
                                .to_ptr<TestStruct>();

    REQUIRE(ptr->end_value == 42);
    REQUIRE(ptr->beg_val == 24);
}

TEST_CASE("memory protection methods")
{
    SECTION("is_accessible()")
    {
        REQUIRE_FALSE(reutl::Addr(static_cast<void*>(nullptr)).is_accessible().value_or(false));
    }
    SECTION("is_readable()")
    {
        auto* const alloc =
            VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_READONLY);
        assert(alloc);

        REQUIRE(reutl::Addr(alloc).is_readable().value_or(false));
        VirtualFree(alloc, NULL, MEM_RELEASE);
    }
    SECTION("is_writable()")
    {
        auto* const alloc =
            VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        assert(alloc);

        REQUIRE(reutl::Addr(alloc).is_writable().value_or(false));
        VirtualFree(alloc, NULL, MEM_RELEASE);
    }
    SECTION("is_executable()")
    {
        auto* const alloc =
            VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE);
        assert(alloc);

        REQUIRE(reutl::Addr(alloc).is_executable().value_or(false));
        VirtualFree(alloc, NULL, MEM_RELEASE);
    }
    SECTION("is_guarded()")
    {
        auto* const alloc = VirtualAlloc(nullptr, sizeof(void*), MEM_RESERVE | MEM_COMMIT,
                                         PAGE_READONLY | PAGE_GUARD);
        assert(alloc);

        REQUIRE(reutl::Addr(alloc).is_guarded().value_or(false));
        VirtualFree(alloc, NULL, MEM_RELEASE);
    }
}

TEST_CASE("offset()")
{
    const auto test_struct = std::make_unique<TestStruct>();

    const auto end = reutl::Addr(test_struct.get()) //
                         .offset(offsetof(TestStruct, end_value));

    REQUIRE(*(end.to_ptr<int32_t>()) == 42);

    // NOLINTNEXTLINE(*-conversion*)
    const auto start = end.offset(-offsetof(TestStruct, end_value));

    REQUIRE(*(start.to_ptr<int32_t>()) == 24);
}

TEST_CASE("deref_instr_rel()")
{
    SECTION("jmp back")
    {
        // 0xE8 - jmp opcode
        // 0xD4FFFFFF - relative offset -42 in little endian representation
        const std::array<uint8_t, 5> jmp_instr = {0xE8, 0xD6, 0xFF, 0xFF, 0xFF};

        const auto abs_addr = reutl::Addr(jmp_instr.data()) //
                                  .deref_instr_rel<std::int32_t>(jmp_instr.size());

        const auto jmp_absolute_addr =
            reinterpret_cast<std::uintptr_t>(jmp_instr.data()) + jmp_instr.size() - 42;

        // NOLINTNEXTLINE (performance-no-int-to-ptr)
        REQUIRE(abs_addr.to_ptr() == reinterpret_cast<void*>(jmp_absolute_addr));
    }
    SECTION("jmp forward")
    {
        // 0xEB - jmp opcode
        // 0x42 - relative jmp offset
        constexpr std::array<uint8_t, 2> jmp_instr = {0xE8, 0x42};

        const auto jmp_absolute_addr =
            reinterpret_cast<std::uintptr_t>(jmp_instr.data()) + jmp_instr.size() + 0x42;

        const auto abs_addr =
            reutl::Addr(jmp_instr.data()).deref_instr_rel<std::int8_t>(jmp_instr.size());

        // NOLINTNEXTLINE (performance-no-int-to-ptr)
        REQUIRE(abs_addr.to_ptr() == reinterpret_cast<void*>(jmp_absolute_addr));
    }
}