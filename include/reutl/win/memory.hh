#ifndef REUTL_MEMORY_HH
#define REUTL_MEMORY_HH

#include <expected>
#include <type_traits>

namespace reutl::win {

enum class ErrCheckMemProtection { FailedVirtualQuery };

[[nodiscard]] auto //
is_accessible_addr(const void* addr) -> bool;

[[nodiscard]] auto //
is_readable_addr(const void* addr) -> bool;

[[nodiscard]] auto //
is_writable_addr(const void* addr) -> bool;

[[nodiscard]] auto //
is_executable_addr(const void* addr) -> bool;

[[nodiscard]] auto //
is_guarded_addr(const void* addr) -> bool;

enum class ErrWriteProtectedMem {
    FailedGetOldProtect,
    MemoryNotAllocated,
    FailedChangeProtect,
    FailedRestoreOldProtect
};

namespace detail {

[[nodiscard]] auto write_protected_mem(void* dst, const void* src, std::size_t size)
    -> std::expected<void, ErrWriteProtectedMem>;

} // namespace detail

template <typename T>
    requires std::is_trivially_copyable_v<T>
[[nodiscard]] auto //
write_protected_mem(void* const dst, const T& src) -> std::expected<void, ErrWriteProtectedMem>
{
    return detail::write_protected_mem(dst, &src, sizeof(src));
}

} // namespace reutl::win

#endif // REUTL_MEMORY_HH