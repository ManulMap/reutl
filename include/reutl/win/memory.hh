#ifndef REUTL_MEMORY_HH
#define REUTL_MEMORY_HH

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

} // namespace reutl::win

#endif // REUTL_MEMORY_HH