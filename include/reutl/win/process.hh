#ifndef REUTL_PROCESS_HH
#define REUTL_PROCESS_HH

#include <string>
#include <expected>
#include <string_view>

namespace reutl::win {

struct ModuleInfo {
    void* begin;
    std::size_t size;
};

enum class ErrGetMdlInfo { MdlNotFound };

[[nodiscard]] auto //
get_module_info(std::wstring_view mdl) -> std::expected<ModuleInfo, ErrGetMdlInfo>;

} // namespace reutl::win

#endif // REUTL_PROCESS_HH
