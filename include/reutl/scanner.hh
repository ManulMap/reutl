#ifndef REUTL_SCANNER_HH
#define REUTL_SCANNER_HH

#include "addr.hh"
#include "reutl/win/process.hh"

#include <string_view>
#include <optional>
#include <array>
#include <algorithm>
#include <charconv>
#include <ranges>
#include <span>
#include <cassert>
#include <expected>
#include <string>

namespace reutl {

namespace detail {

template <std::size_t size>
struct FixedString {
    constexpr FixedString(const char (&array)[size]) noexcept
    {
        std::ranges::copy(array, str.begin());
    }

    constexpr operator std::string_view() const noexcept
    {
        return {str.data(), size - 1};
    }

    std::array<char, size> str; // NOLINT(misc-non-private-member-variables-in-classes)
};

constexpr auto to_int = [](std::string_view sv) -> std::optional<std::uint8_t> {
    if (sv == "??")
        return std::nullopt;

    std::uint8_t value{};
    const auto res = std::from_chars(sv.data(), sv.data() + sv.size(), value, 16);
    assert(res.ec == std::errc{});

    return value;
};

constexpr auto to_sv = [](auto&& rng) { return std::string_view(rng); };

template <FixedString pattern>
[[nodiscard]] constexpr auto pat2mask()
    -> std::array<std::optional<std::uint8_t>, pattern.str.size() / 3>
{
    const std::size_t arr_size = pattern.str.size() / 3;
    constexpr auto pat         = std::string_view{pattern};
    std::array<std::optional<std::uint8_t>, arr_size> res_mut{};

    static_assert(std::ranges::distance(std::views::split(pat, ' ')) == arr_size);

    for (std::size_t i{};                                     //
         const auto byte : std::views::split(pat, ' ')        //
                               | std::views::transform(to_sv) //
                               | std::views::transform(to_int))
    {
        res_mut.at(i++) = byte;
    }

    return res_mut;
}

[[nodiscard]] auto                                 //
find_pattern(std::span<const std::uint8_t> memory, //
             std::span<const std::optional<std::uint8_t>> mask) -> std::optional<Addr>;

} // namespace detail

template <detail::FixedString pattern> // NOLINT(clang-diagnostic-ctad-maybe-unsupported)
[[nodiscard]] auto                     //
find_pattern(const void* const begin, const std::size_t size) -> std::optional<Addr>
{
    const auto mask = detail::pat2mask<pattern>();
    return detail::find_pattern(std::span{static_cast<const std::uint8_t*>(begin), size},
                                std::span{mask.data(), mask.size()});
}

enum class ErrFindMdlPattern { ErrGetMdlInfo };

template <detail::FixedString pattern> // NOLINT(clang-diagnostic-ctad-maybe-unsupported)
[[nodiscard]] auto find_pattern_in_module(std::wstring_view mdl)
    -> std::expected<std::optional<Addr>, ErrFindMdlPattern>
{
    const auto mdl_info = win::get_module_info(mdl);

    if (!mdl_info)
        return std::unexpected(ErrFindMdlPattern::ErrGetMdlInfo);

    return find_pattern<pattern>(mdl_info.value().begin, mdl_info.value().size);
}

} // namespace reutl

#endif // REUTL_SCANNER_HH