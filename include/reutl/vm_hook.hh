#ifndef REUTL_VM_HOOK_HH
#define REUTL_VM_HOOK_HH

#include "win/memory.hh"

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <memory>
#include <expected>

namespace reutl {

class VmHook {
public: // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    enum class ErrMakeVmHook { FailedChangeVTable };

    friend auto //
    make_vm_hook(void* vmt, std::uint32_t index, const void* hook)
        -> std::expected<std::unique_ptr<VmHook>, ErrMakeVmHook>;

    VmHook(const VmHook&) = default;
    VmHook(VmHook&&)      = default;

    auto operator=(const VmHook&) -> VmHook& = default;
    auto operator=(VmHook&&) -> VmHook&      = default;

    ~VmHook() // NOLINT(modernize-use-equals-default) wtf? stupid clang-tidy!
    {
        if (!win::write_protected_mem(static_cast<void**>(vmt_) + index_, orig_))
            std::abort();
    }

    [[nodiscard]] auto get_orig() const -> void*
    {
        return orig_;
    }

private:
    explicit VmHook(void* const vmt, const std::uint32_t index) noexcept : vmt_{vmt}, index_{index}
    {}

    void* vmt_;
    std::uint32_t index_;
    void* orig_ = nullptr;
};

[[nodiscard]] inline auto //
make_vm_hook(void* const vmt, const std::uint32_t index, const void* const hook)
    -> std::expected<std::unique_ptr<VmHook>, VmHook::ErrMakeVmHook>
{
    auto* const orig_in_vmt = static_cast<void**>(vmt) + index;
    void* const original_vm = *orig_in_vmt; // NOLINT(clang-analyzer-core.NullDereference)

    if (!win::write_protected_mem(orig_in_vmt, hook))
        return std::unexpected(VmHook::ErrMakeVmHook::FailedChangeVTable);

    auto ret   = std::unique_ptr<VmHook>(new VmHook(vmt, index));
    ret->orig_ = original_vm;

    return ret;
}

} // namespace reutl

#endif // REUTL_VM_HOOK_HH