#ifndef REUTL_PROCESS_HH
#define REUTL_PROCESS_HH

#include <string>
#include <expected>

namespace reutl::win {

struct ModuleInfo {
    void* begin;
    std::size_t size;
};

enum class ErrGetMdlInfo { MdlNotFound, NoAccessToMdlInfo };

[[nodiscard]] auto //
get_module_info(const std::string& mdl) -> std::expected<ModuleInfo, ErrGetMdlInfo>;

} // namespace reutl::win

#endif // REUTL_PROCESS_HH
