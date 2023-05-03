#include "reutl/safe_vmt.hh"

namespace reutl {

SafeVmt::SafeVmt(void* object_ptr) noexcept
    : object_ptr_{object_ptr}, original_vmt_{*static_cast<const void***>(object_ptr)}
{
    const auto* const rtti = original_vmt_[-1];

    has_rtti_ = rtti != nullptr;

    std::size_t vms_cnt_mut = 0;
    for (; Addr(original_vmt_[vms_cnt_mut]).is_executable().value_or(false); ++vms_cnt_mut) {
    }
    vms_cnt_ = vms_cnt_mut;

    if (!vms_cnt_mut)
        return;

    allocated_mem_ = std::make_unique<void*[]>(vms_cnt_mut + (has_rtti_ ? 1 : 0));

    std::memcpy(allocated_mem_.get(), original_vmt_ - (has_rtti_ ? 1 : 0),
                (vms_cnt_mut + (has_rtti_ ? 1 : 0)) * sizeof(void*));

    *static_cast<void**>(object_ptr_) = allocated_mem_.get() + (has_rtti_ ? 1 : 0);
}

} // namespace reutl
