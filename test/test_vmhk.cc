#include "reutl/vm_hook.hh"

#include <catch2/catch_test_macros.hpp>
#include <memory>

struct A {
    virtual auto mul2(int x) -> int
    {
        return 2 * x;
    }
};

std::unique_ptr<reutl::VmHook> g_mul_hk = nullptr;

using Mul2 = auto (*)(A* self, int) -> int;

auto hook([[maybe_unused]] A* const self, int x) -> int
{
    return reinterpret_cast<Mul2>(g_mul_hk->get_orig())(self, x + 2);
}

TEST_CASE("vm hook")
{
    const auto a_ptr  = std::make_unique<A>();
    const auto mul2_5 = a_ptr->mul2(5);
    REQUIRE(mul2_5 == 10);

    g_mul_hk = reutl::make_vm_hook(*reinterpret_cast<void**>(a_ptr.get()), 0,
                                   reinterpret_cast<void*>(hook))
                   .value_or(nullptr);

    const auto mul2_5_hk = a_ptr->mul2(5);
    REQUIRE(mul2_5_hk == 14);
}