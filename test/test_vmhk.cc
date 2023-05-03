#include "reutl/vm_hook.hh"
#include "reutl/safe_vmt.hh"

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

struct Poly {
    virtual auto add_10(int x) -> int
    {
        return x + 10;
    }

    virtual auto sub_10(int x) -> int
    {
        return x - 10;
    }
};

std::unique_ptr<reutl::VmHook> g_add_hk    = nullptr;
std::unique_ptr<reutl::SafeVmt> g_poly_vmt = nullptr;

using Add10 = auto (*)(Poly* self, int) -> int;

auto add_cb(Poly* self, int x) -> int
{
    return reinterpret_cast<Add10>(g_add_hk->get_orig())(self, x + 10);
};

using Sub10 = auto (*)(Poly* self, int) -> int;

auto sub_cb(Poly* self, int x) -> int
{
    return reinterpret_cast<Sub10>(g_poly_vmt->get_orig(1).value())(self, x + 10);
}

TEST_CASE("safe vmt")
{
    const auto poly_ptr = std::make_unique<Poly>();

    SECTION("manually setted hooks")
    {
        const auto add10_6 = poly_ptr->add_10(6);

        REQUIRE(add10_6 == 16);

        const auto vmt = reutl::SafeVmt(poly_ptr.get());

        g_add_hk = reutl::make_vm_hook(vmt.get_ptr(), 0, reinterpret_cast<void*>(add_cb)) //
                       .value_or(nullptr);

        const auto add10_6_hk = poly_ptr->add_10(6);
        REQUIRE(add10_6_hk == 26);
    }
    SECTION("cleanup test")
    {
        // should return right value cause vmt destructed
        const auto add10_6 = poly_ptr->add_10(6);

        REQUIRE(add10_6 == 16);
    }
    SECTION("setting hooks using SafeVmt methods")
    {
        const auto sub10_42 = poly_ptr->sub_10(42);

        REQUIRE(sub10_42 == 32);

        g_poly_vmt = std::make_unique<reutl::SafeVmt>(poly_ptr.get());

        g_poly_vmt->install_hook(1, reinterpret_cast<void*>(sub_cb));

        const auto sub10_42_hk = poly_ptr->sub_10(42);
        REQUIRE(sub10_42_hk == 42);

        g_poly_vmt->release_hook(1);

        const auto sub10_42_rel_hk = poly_ptr->sub_10(42);
        REQUIRE(sub10_42_rel_hk == 32);
    }
}