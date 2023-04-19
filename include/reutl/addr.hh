#ifndef REUTL_ADDR_HH
#define REUTL_ADDR_HH

#include <cstdint>
#include <type_traits>

namespace reutl {

class Addr {
public:
    friend auto make_addr(auto* ptr) -> Addr;

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