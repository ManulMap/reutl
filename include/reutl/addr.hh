#ifndef REUTL_ADDR_HH
#define REUTL_ADDR_HH

#include "win/memory.hh"

#include <cstdint>
#include <cassert>
#include <concepts>
#include <compare>
#include <expected>

namespace reutl {

template <typename T>
concept RelAddrType = std::same_as<std::remove_cv_t<T>, std::int8_t> ||
                      std::same_as<std::remove_cv_t<T>, std::int16_t> ||
                      std::same_as<std::remove_cv_t<T>, std::int32_t>;

class Addr {
public:
    explicit Addr(const void* const ptr) noexcept : ptr_{ptr}
    {}

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

        return Addr{static_cast<const uint8_t*>(ptr_) + offs};
    }

    template <RelAddrType T> // NOLINT(readability-identifier-naming)
    [[nodiscard]] auto deref_instr_rel(const std::size_t instr_size) const -> Addr
    {
        assert(instr_size >= sizeof(T));

        const auto opc_size = instr_size - sizeof(T);
        const auto rel      = this->offset(opc_size);

        assert(rel.is_readable());

        const auto rel_offs = *static_cast<const T*>(rel.ptr_);
        return this->offset(rel_offs + instr_size);
    }

    // NOLINTEND(*-sign-co*)

    template <typename T = void>
    [[nodiscard]] auto to_ptr() const -> T*
    {
        return static_cast<T*>(const_cast<void*>(ptr_));
    }

    enum class ErrCheckMemAccess { WinapiFailed };

    [[nodiscard]] auto is_accessible() const -> std::expected<bool, ErrCheckMemAccess>
    {
        return win::is_accessible_addr(ptr_) //
            .transform_error([](const auto err) { return ErrCheckMemAccess::WinapiFailed; });
    }

    [[nodiscard]] auto is_readable() const -> std::expected<bool, ErrCheckMemAccess>
    {
        return win::is_readable_addr(ptr_) //
            .transform_error([](const auto err) { return ErrCheckMemAccess::WinapiFailed; });
    }

    [[nodiscard]] auto is_writable() const -> std::expected<bool, ErrCheckMemAccess>
    {
        return win::is_writable_addr(ptr_) //
            .transform_error([](const auto err) { return ErrCheckMemAccess::WinapiFailed; });
    }

    [[nodiscard]] auto is_executable() const -> std::expected<bool, ErrCheckMemAccess>
    {
        return win::is_executable_addr(ptr_) //
            .transform_error([](const auto err) { return ErrCheckMemAccess::WinapiFailed; });
    }

    [[nodiscard]] auto is_guarded() const -> std::expected<bool, ErrCheckMemAccess>
    {
        return win::is_guarded_addr(ptr_) //
            .transform_error([](const auto err) { return ErrCheckMemAccess::WinapiFailed; });
    }

private:
    const void* ptr_;
};

} // namespace reutl

#endif // REUTL_ADDR_HH