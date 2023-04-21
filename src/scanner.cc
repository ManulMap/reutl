#include "reutl/scanner.hh"

namespace reutl::detail {

[[nodiscard]] auto //
find_pattern(std::span<const std::uint8_t> memory,
             const std::span<const std::optional<std::uint8_t>> mask) -> std::optional<Addr>
{
    auto pred = [](const std::uint8_t mem, const std::optional<std::uint8_t> mask_byte) {
        return !mask_byte.has_value() || mem == mask_byte;
    };
    const auto res = std::ranges::search(memory, mask, pred);

    if (res.empty())
        return std::nullopt;

    return make_addr(res.data());
}

} // namespace reutl::detail