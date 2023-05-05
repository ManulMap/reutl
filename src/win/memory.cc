#include "reutl/win/memory.hh"

#include <Windows.h>
#include <cassert>
#include <cstring>

namespace reutl::win {

[[nodiscard]] auto //
is_accessible_addr(const void* const addr) -> std::expected<bool, ErrCheckMemProtection>
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return std::unexpected(ErrCheckMemProtection::FailedVirtualQuery);

    return mbi.Protect ^ PAGE_NOACCESS;
}

[[nodiscard]] auto //
is_readable_addr(const void* const addr) -> std::expected<bool, ErrCheckMemProtection>
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return std::unexpected(ErrCheckMemProtection::FailedVirtualQuery);

    const auto prt = mbi.Protect;

    return prt & PAGE_READONLY || prt & PAGE_READWRITE || prt & PAGE_EXECUTE_READ ||
           prt & PAGE_EXECUTE_READWRITE;
}

[[nodiscard]] auto //
is_writable_addr(const void* const addr) -> std::expected<bool, ErrCheckMemProtection>
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return std::unexpected(ErrCheckMemProtection::FailedVirtualQuery);

    const auto prt = mbi.Protect;
    return prt & PAGE_WRITECOPY || prt & PAGE_READWRITE || prt & PAGE_EXECUTE_WRITECOPY ||
           prt & PAGE_EXECUTE_READWRITE;
}

[[nodiscard]] auto //
is_executable_addr(const void* const addr) -> std::expected<bool, ErrCheckMemProtection>
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return std::unexpected(ErrCheckMemProtection::FailedVirtualQuery);

    const auto prt = mbi.Protect;

    return prt & PAGE_EXECUTE || prt & PAGE_EXECUTE_READ || prt & PAGE_EXECUTE_READWRITE ||
           prt & PAGE_EXECUTE_WRITECOPY;
}

[[nodiscard]] auto //
is_guarded_addr(const void* const addr) -> std::expected<bool, ErrCheckMemProtection>
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return std::unexpected(ErrCheckMemProtection::FailedVirtualQuery);

    return mbi.Protect & PAGE_GUARD;
}

namespace detail {

[[nodiscard]] auto //
write_protected_mem(void* const dst, const void* const src, const std::size_t size)
    -> std::expected<void, ErrWriteProtectedMem>
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(dst, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return std::unexpected(ErrWriteProtectedMem::FailedGetOldProtect);

    if (mbi.Protect == PAGE_NOACCESS)
        return std::unexpected(ErrWriteProtectedMem::MemoryNotAllocated);

    DWORD temp_prot = PAGE_EXECUTE_READWRITE;
    if (!VirtualProtect(dst, size, temp_prot, &mbi.Protect))
        return std::unexpected(ErrWriteProtectedMem::FailedChangeProtect);

    std::memcpy(dst, src, size);

    if (!VirtualProtect(dst, size, mbi.Protect, &temp_prot))
        return std::unexpected(ErrWriteProtectedMem::FailedRestoreOldProtect);

    return std::expected<void, ErrWriteProtectedMem>{};
}

} // namespace detail

} // namespace reutl::win