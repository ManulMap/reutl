#ifndef REUTL_SAFE_VMT_HH
#define REUTL_SAFE_VMT_HH

#include "reutl/addr.hh"
#include "vm_hook.hh"

#include <memory>
#include <cstring>
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <utility>

namespace reutl {

class SafeVmt {
public:
    explicit SafeVmt(void* object_ptr) noexcept;

    [[nodiscard]] auto get_ptr() const -> void*
    {
        return allocated_mem_.get() + (has_rtti_ ? 1 : 0);
    }

    auto install_hook(std::size_t index, void* hook) -> void
    {
        assert(index < vms_cnt_);
        assert(!hooks_.contains(index));

        hooks_.emplace(index, make_vm_hook(allocated_mem_.get() + (has_rtti_ ? 1 : 0), index, hook)
                                  .value_or(nullptr));
    }

    auto release_hook(std::size_t index) -> void
    {
        assert(index < vms_cnt_);
        assert(hooks_.contains(index));

        hooks_.erase(index);
    }

    [[nodiscard]] auto get_orig(std::size_t index) const -> std::optional<void*>
    {
        if (hooks_.contains(index))
            return hooks_.at(index)->get_orig();

        return std::nullopt;
    }

    [[nodiscard]] auto get_vms_cnt() const -> std::size_t
    {
        return vms_cnt_;
    }

    SafeVmt(const SafeVmt&)              = delete;
    auto operator=(SafeVmt&) -> SafeVmt& = delete;

    SafeVmt(SafeVmt&&)                    = default;
    auto operator=(SafeVmt&&) -> SafeVmt& = default;

    ~SafeVmt()
    {
        *static_cast<void**>(object_ptr_) = original_vmt_;
    }

private:
    bool has_rtti_;
    void* object_ptr_;
    const void** original_vmt_;
    std::unique_ptr<void*[]> allocated_mem_;
    std::size_t vms_cnt_;

    std::unordered_map<std::size_t, std::unique_ptr<VmHook>> hooks_{};
};

} // namespace reutl

#endif // REUTL_SAFE_VMT_HH