#ifndef REUTL_ADDR_HH
#define REUTL_ADDR_HH

#include "win/memory.hh"

#include <cstdint>
#include <cassert>
#include <type_traits>
#include <concepts>
#include <compare>

namespace reutl {

template <typename T>
concept RelAddrType = std::same_as<std::remove_cv_t<T>, std::int8_t> ||
                      std::same_as<std::remove_cv_t<T>, std::int16_t> ||
                      std::same_as<std::remove_cv_t<T>, std::int32_t>;

class Addr {
public:
    friend auto make_addr(auto* ptr) -> Addr;

    [[nodiscard]] auto operator<=>(const Addr&) const        = default;
    [[nodiscard]] auto operator==(const Addr&) const -> bool = default;

    [[nodiscard]] auto operator<=>(const void* const rhs) const noexcept
    {
        return rhs <=> ptr_;
    }

    [[nodiscard]] auto operator==(const void* const rhs) const noexcept -> bool
    {
        return rhs == ptr_;
    }

    // NOLINTBEGIN(*-sign-co*)
    [[nodiscard]] auto offset(const std::ptrdiff_t offs) const -> Addr
    {
        {
            // check for overflows // NOLINTNEXTLINE(readability-qualified-auto)
            const auto ptr = reinterpret_cast<std::uintptr_t>(ptr_);
            assert(offs < 0 ? ptr > -offs : ptr + offs >= ptr);
        }

        return Addr{static_cast<uint8_t*>(ptr_) + offs};
    }

    template <RelAddrType T> // NOLINT(readability-identifier-naming)
    [[nodiscard]] auto deref_rel(const std::size_t instr_size) const -> Addr
    {
        assert(instr_size >= sizeof(T));

        const auto opc_size = instr_size - sizeof(T);
        const auto rel      = this->offset(opc_size);

        assert(rel.is_readable());

        const auto rel_offs = *static_cast<T*>(rel.ptr_);
        return this->offset(rel_offs + instr_size);
    }

    // NOLINTEND(*-sign-co*)

    template <typename T = void>
    [[nodiscard]] auto to_ptr() const -> T*
    {
        return static_cast<T*>(ptr_);
    }

    [[nodiscard]] auto is_accessible() const -> bool
    {
        return win::is_accessible_addr(ptr_);
    }

    [[nodiscard]] auto is_readable() const -> bool
    {
        return win::is_readable_addr(ptr_);
    }

    [[nodiscard]] auto is_writable() const -> bool
    {
        return win::is_writable_addr(ptr_);
    }

    [[nodiscard]] auto is_executable() const -> bool
    {
        return win::is_executable_addr(ptr_);
    }

    [[nodiscard]] auto is_guarded() const -> bool
    {
        return win::is_guarded_addr(ptr_);
    }

private:
    explicit Addr(void* const ptr) noexcept : ptr_{ptr}
    {}

    void* ptr_;
};

[[nodiscard]] auto make_addr(auto* const ptr) -> Addr
{
    auto* const non_const_ptr = const_cast<std::add_pointer_t< //
        std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>>>(ptr);

    return Addr{non_const_ptr};
}

} // namespace reutl

#endif // REUTL_ADDR_HH