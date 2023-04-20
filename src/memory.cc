#include "reutl/win/memory.hh"

#include <Windows.h>
#include <cassert>

namespace reutl::win {

[[nodiscard]] auto //
is_accessible_addr(const void* const addr) -> bool
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    return mbi.Protect ^ PAGE_NOACCESS;
}

[[nodiscard]] auto //
is_readable_addr(const void* const addr) -> bool
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    const auto prt = mbi.Protect;

    return prt & PAGE_READONLY || prt & PAGE_READWRITE || prt & PAGE_EXECUTE_READ ||
           prt & PAGE_EXECUTE_READWRITE;
}

[[nodiscard]] auto //
is_writable_addr(const void* const addr) -> bool
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    const auto prt = mbi.Protect;
    return prt & PAGE_WRITECOPY || prt & PAGE_READWRITE || prt & PAGE_EXECUTE_WRITECOPY ||
           prt & PAGE_EXECUTE_READWRITE;
}

[[nodiscard]] auto //
is_executable_addr(const void* const addr) -> bool
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    const auto prt = mbi.Protect;

    return prt & PAGE_EXECUTE || prt & PAGE_EXECUTE_READ || prt & PAGE_EXECUTE_READWRITE ||
           prt & PAGE_EXECUTE_WRITECOPY;
}

[[nodiscard]] auto //
is_guarded_addr(const void* const addr) -> bool
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

    return mbi.Protect & PAGE_GUARD;
}

} // namespace reutl::win