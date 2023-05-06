#include <reutl/safe_vmt.hh>
#include <reutl/scanner.hh>

#include <windows.h>
#include <Psapi.h>
#include <thread>
#include <iostream>
#include <memory>
#include <cstdint>

enum NetChannelBufType : int {
    K_BUF_DEFAULT    = -1,
    K_BUF_UNRELIABLE = 0,
    K_BUF_RELIABLE   = 1,
    K_BUF_VOICE      = 2,
};

struct CNetworkSerializerPB {
    const char* unscoped_name;
    std::uint32_t category_mask;
    [[maybe_unused]] int unk;
    void* pb_binding;
    const char* group_name;
    std::int16_t msg_id;
    std::uint8_t group_id;
    std::uint8_t default_buf_type;
    [[maybe_unused]] std::uint8_t pad[28];
};

std::unique_ptr<reutl::VmHook> g_recv_msg_hk = nullptr;
std::unique_ptr<reutl::VmHook> g_send_msg_hk = nullptr;

auto hook_send_recv_unsafe() -> void
{
    const auto net_chan_vmt =
        reutl::find_pattern_in_module<"40 53 56 57 41 56 48 83 EC ?? 45 33 F6 48 8D 71">(
            "networksystem.dll")
            .value_or(std::nullopt)
            .transform([](const reutl::Addr addr) {
                return addr
                    .offset(0x15) //
                    .deref_instr_rel<std::int32_t>(7)
                    .to_ptr();
            });

    if (!net_chan_vmt) {
        std::cout << "NetChannel signature not found\n";
        return;
    }

    using RecvMsg = auto (*)(void*, CNetworkSerializerPB*, void*, void*, int)->void;

    auto recv_msg_cb = [](void* self, CNetworkSerializerPB* msg_handle, void* pb_msg, void* type,
                          const int bits) -> void {
        std::cout << "recv: " << msg_handle->msg_id << '\n';

        const auto orig = static_cast<RecvMsg>(g_recv_msg_hk->get_orig());
        return orig(self, msg_handle, pb_msg, type, bits);
    };

    g_recv_msg_hk =
        reutl::make_vm_hook(net_chan_vmt.value(), 0x56, static_cast<RecvMsg>(recv_msg_cb))
            .value_or(nullptr);

    if (!g_recv_msg_hk) {
        std::cout << "Error occurred while installing PostReceivedNetMessage hook\n";
        std::abort();
    }

    using SendMsg = auto (*)(void*, CNetworkSerializerPB*, void*, NetChannelBufType)->bool;

    auto send_msg_cb = [](void* self, CNetworkSerializerPB* msg_handle, void* pb_msg,
                          NetChannelBufType buf_type) -> bool {
        std::cout << "send: " << msg_handle->msg_id << '\n';
        const auto orig = static_cast<SendMsg>(g_send_msg_hk->get_orig());

        return orig(self, msg_handle, pb_msg, buf_type);
    };

    g_send_msg_hk =
        reutl::make_vm_hook(net_chan_vmt.value(), 0x45, static_cast<SendMsg>(send_msg_cb))
            .value_or(nullptr);

    if (!g_send_msg_hk) {
        std::cout << "Error occurred while installing PostReceivedNetMessage hook\n";
        std::abort();
    }
}

std::unique_ptr<reutl::SafeVmt> g_nw_sys_vmt = nullptr;

auto safe_net_chan_hk() -> void
{
    using CreateInterface = auto (*)(const char*, int*)->void*;

    const auto create_interface = reinterpret_cast<CreateInterface>(
        GetProcAddress(GetModuleHandleA("networksystem.dll"), "CreateInterface"));

    void* nw_sys = create_interface("NetworkSystemVersion001", nullptr);

    g_nw_sys_vmt = std::make_unique<reutl::SafeVmt>(nw_sys);

    using CreateNetChan = auto (*)(void*, int, void*, const char*, unsigned, unsigned)->void*;

    auto create_net_chan_cb = [](void* self, int unk, void* ns_addr, const char* str, unsigned unk2,
                                 unsigned unk3) -> void* {
        std::cout << "NetChannel created\n";
        const auto orig = static_cast<CreateNetChan>(g_nw_sys_vmt->get_orig(0x1A).value());

        return orig(self, unk, ns_addr, str, unk2, unk3);
    };

    g_nw_sys_vmt->install_hook(0x1A, static_cast<CreateNetChan>(create_net_chan_cb));
}

auto on_inject() -> void
{
    safe_net_chan_hk();
    hook_send_recv_unsafe();
}

auto WINAPI DllMain(HINSTANCE dll, DWORD call_reason, LPVOID reserved) -> BOOL
{
    switch (call_reason) {
    case DLL_PROCESS_ATTACH: {
        AllocConsole();
        freopen_s(reinterpret_cast<FILE**> stdout, "CONOUT$", "w", stdout);
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(on_inject), 0, 0, 0);
        break;
    }
    case DLL_PROCESS_DETACH:
        if (reserved != nullptr)
            break; // do not do cleanup if process termination scenario

        // Perform any necessary cleanup.
        fclose(stdout);
        FreeConsole();
        break;
    default:
        break;
    }
    return TRUE;
}
