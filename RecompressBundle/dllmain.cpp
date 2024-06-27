// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <thread>
struct BuildCompression {
    uint32_t compression;
    uint32_t level;
};

struct Priority {
    uint32_t priority;
};
struct Il2CppString {
    uint32_t length;
    wchar_t chars[1]; // This is a flexible array member; actual size is (length + 1)
};
const char* WaitForUA() {
    while (true) {
        if (GetModuleHandleA("GameAssembly.dll") != NULL) {
            printf("Base address of %s: %p\n", "UA", GetModuleHandleA("UserAssembly.dll"));
            return "UA";
        }

        Sleep(100);
    }
}

std::vector<uint8_t> hexStringToByteArray(const std::string& hexString) {
    std::vector<uint8_t> byteArray;
    for (size_t i = 0; i < hexString.length(); i += 2) {
        byteArray.push_back(static_cast<uint8_t>(std::stoi(hexString.substr(i, 2), nullptr, 16)));
    }
    return byteArray;
}

// Function to create an Il2Cpp string manually
Il2CppString* CreateIl2CppString(const std::string& str) {
    size_t len = str.length();
    size_t size = sizeof(Il2CppString) + (len * sizeof(wchar_t));
    Il2CppString* il2CppStr = (Il2CppString*)malloc(size);
    if (il2CppStr) {
        il2CppStr->length = static_cast<uint32_t>(len);
        mbstowcs_s(nullptr, il2CppStr->chars, len + 1, str.c_str(), len);
        il2CppStr->chars[len] = L'\0';
    }
    return il2CppStr;
}

// Define function pointer types
typedef void* (*Il2CppResolveICall_t)(const char*);
typedef void (*Il2CppInitFunc)();
//typedef void* (*RecompressAssetBundleAsync_Internal_Injected_t)(Il2CppString*, Il2CppString*, BuildCompression*, uint32_t, Priority*);
typedef void* (*RecompressAssetBundleAsync_Internal_Injected_t)(void*, void*, BuildCompression*, uint32_t, Priority*);
typedef const char* (*GetHumanReadableResult_t)(void*);
typedef void* (*Il2CppStringNew_t)(const char*);
// Function pointers
Il2CppResolveICall_t il2cpp_resolve_icall = nullptr;
RecompressAssetBundleAsync_Internal_Injected_t RecompressAssetBundleAsync_Internal_Injected = nullptr;
GetHumanReadableResult_t GetHumanReadableResult = nullptr;
Il2CppStringNew_t il2cpp_string_new = nullptr;
Il2CppInitFunc il2cpp_init = nullptr;
// Function to attach to an existing console or create a new one
void AttachConsole() {
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }

    FILE* pFile = nullptr;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    freopen_s(&pFile, "CONOUT$", "w", stderr);
    freopen_s(&pFile, "CONIN$", "r", stdin);
}
void WaitForDebugger() {
    while (!IsDebuggerPresent()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void ProcessAttach() {
    WaitForUA();

    HMODULE module = GetModuleHandleA("GameAssembly.dll");
    if (!module) {
        std::cerr << "Failed to get module handle for GameAssembly.dll" << std::endl;
        return;
    }
    std::cout << "HEELOEOE: "<< std::endl;


    il2cpp_init = reinterpret_cast<Il2CppInitFunc>(GetProcAddress(module, "il2cpp_init"));

    if (!il2cpp_init) {
        std::cerr << "Failed to resolve il2cpp_init function" << std::endl;
        return;
    }

    // Call il2cpp_init
    il2cpp_init();
    // Resolve il2cpp_resolve_icall
    il2cpp_resolve_icall = (Il2CppResolveICall_t)GetProcAddress(module, "il2cpp_resolve_icall");
    if (!il2cpp_resolve_icall) {
        std::cerr << "Failed to resolve il2cpp_resolve_icall" << std::endl;
        return;
    }
    // Resolve il2cpp_string_new
    il2cpp_string_new = (Il2CppStringNew_t)GetProcAddress(module, "il2cpp_string_new");
    if (!il2cpp_string_new) {
        std::cerr << "Failed to resolve il2cpp_string_new" << std::endl;
        return;
    }

    // Resolve the necessary functions using il2cpp_resolve_icall
    RecompressAssetBundleAsync_Internal_Injected = (RecompressAssetBundleAsync_Internal_Injected_t)il2cpp_resolve_icall("UnityEngine.AssetBundle::RecompressAssetBundleAsync_Internal_Injected");
    GetHumanReadableResult = (GetHumanReadableResult_t)il2cpp_resolve_icall("UnityEngine.AssetBundleRecompressOperation::get_humanReadableResult");

    if (!RecompressAssetBundleAsync_Internal_Injected || !GetHumanReadableResult) {
        std::cerr << "Failed to resolve functions" << std::endl;
        return;
    }

    std::string path =R"(D:\UnityShit\a\dummygame\My project(1)_Data\StreamingAssets\meiiiiiiiiiiiiiiiiiii)";
    std::string out = R"("D:\UnityShit\a\dummygame\My project(1)_Data\StreamingAssets\meiiiiiiiiiiiiiiiiiii_deccccc)";

    BuildCompression buildCompression;
    buildCompression.compression = 0x00000000; // Replace with actual value
    buildCompression.level = 0x02000000; // Replace with actual value

    Priority priority;
    priority.priority = 0x04000000; // Replace with actual value

    // Create Il2Cpp strings
    //Il2CppString* il2cppPath = CreateIl2CppString(path);
    //Il2CppString* il2cppOut = CreateIl2CppString(out);
    Sleep(5);
    void* il2cppPath = il2cpp_string_new(path.c_str());
    void* il2cppOut = il2cpp_string_new(out.c_str());
    // Call the RecompressAssetBundleAsync function
    void* z = RecompressAssetBundleAsync_Internal_Injected(il2cppPath, il2cppOut, &buildCompression, 0, &priority);
    printf("aaaaaaaaa %p",z);
    // Uncomment to simulate the get_humanReadableResult function call
    // void* result = GetHumanReadableResult(z);
    // std::string str = static_cast<char*>(result);
    // std::cout << "Result: " << str << std::endl;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        AttachConsole();
        printf("WTQWRWEQ");
        //WaitForDebugger();
        //ProcessAttach();
        std::thread(ProcessAttach).detach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
