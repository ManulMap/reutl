#include "reutl/win/process.hh"

#include <Windows.h>
#include <Psapi.h>

namespace reutl::win {

[[nodiscard]] auto //
get_module_info(const std::string& mdl) -> std::expected<ModuleInfo, ErrGetMdlInfo>
{
    const HMODULE module = GetModuleHandleA(mdl.c_str());

    if (!module)
        return std::unexpected(ErrGetMdlInfo::MdlNotFound);

    MODULEINFO mdl_inf_mut;
    ZeroMemory(&mdl_inf_mut, sizeof(MODULEINFO));

    if (!GetModuleInformation(GetCurrentProcess(), module, &mdl_inf_mut, sizeof(MODULEINFO)))
        return std::unexpected(ErrGetMdlInfo::NoAccessToMdlInfo);

    return ModuleInfo{mdl_inf_mut.lpBaseOfDll, mdl_inf_mut.SizeOfImage};
}

} // namespace reutl::win