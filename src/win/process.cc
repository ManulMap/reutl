#include "reutl/win/process.hh"

#include <Windows.h>
#include <Psapi.h>
#include <codecvt>
#include <expected>
#include <locale>
#include <string.h>
#include <string_view>
#include <winnt.h>
#include <winternl.h>

namespace reutl::win {

[[nodiscard]] auto //
get_module_info(std::wstring_view mdl) -> std::expected<ModuleInfo, ErrGetMdlInfo>
{
    auto peb = reinterpret_cast<PEB*>(__readgsqword(0x60));
    auto ldr = peb->Ldr;
    auto listhead = &(ldr->InMemoryOrderModuleList);
    auto listentry = listhead->Flink;
    while (listentry != listhead)
    {
        auto entry = CONTAINING_RECORD(listentry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        auto module_name = std::wstring_view { entry->FullDllName.Buffer };
        if (module_name.contains( mdl )) {
            auto handle = reinterpret_cast<HMODULE>(entry->DllBase);
            auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
            auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(((uint8_t*)handle) + dos->e_lfanew);
            return ModuleInfo{entry->DllBase, nt->OptionalHeader.SizeOfImage};
        }
        listentry = listentry->Flink;
    }

    return std::unexpected(ErrGetMdlInfo::MdlNotFound);
}

} // namespace reutl::win