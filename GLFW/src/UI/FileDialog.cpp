// Isolated here so no other translation unit needs to deal with <Windows.h>/COM headers.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define INITGUID // define CLSID_FileOpenDialog/IID_IFileOpenDialog here instead of needing Uuid.lib
#include <shobjidl.h>
#include <wrl/client.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "FileDialog.h"

using Microsoft::WRL::ComPtr;

namespace {
    std::string wideToUtf8(const wchar_t* wide) {
        if (!wide) return "";
        int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
        if (size <= 0) return "";
        std::string result(static_cast<size_t>(size - 1), '\0'); // size includes the null terminator
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), size, nullptr, nullptr);
        return result;
    }
}

std::string FileDialog::openModelFile(GLFWwindow* owner) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    // S_OK/S_FALSE both mean this call incremented COM's per-thread init count and must be
    // balanced by CoUninitialize(). RPC_E_CHANGED_MODE means COM was already initialized on
    // this thread with a different (still usable) apartment model -- this call had no effect.
    bool needsUninit = (hr == S_OK || hr == S_FALSE);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return "";
    }

    std::string result;
    ComPtr<IFileOpenDialog> dialog;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)))) {
        const COMDLG_FILTERSPEC filters[] = {
            { L"Model Files", L"*.obj;*.gltf;*.glb;*.fbx" },
            { L"All Files", L"*.*" }
        };
        dialog->SetFileTypes(ARRAYSIZE(filters), filters);
        dialog->SetFileTypeIndex(1); // 1-based
        dialog->SetTitle(L"Import Model");

        HWND hwndOwner = owner ? glfwGetWin32Window(owner) : nullptr;
        HRESULT showResult = dialog->Show(hwndOwner);
        if (SUCCEEDED(showResult)) {
            ComPtr<IShellItem> item;
            if (SUCCEEDED(dialog->GetResult(&item))) {
                PWSTR pathPtr = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &pathPtr))) {
                    result = wideToUtf8(pathPtr);
                    CoTaskMemFree(pathPtr);
                }
            }
        }
        // showResult == HRESULT_FROM_WIN32(ERROR_CANCELLED) when the user just cancelled --
        // not an error, `result` simply stays empty.
    }

    if (needsUninit) {
        CoUninitialize();
    }

    for (char& c : result) {
        if (c == '\\') c = '/';
    }
    return result;
}
