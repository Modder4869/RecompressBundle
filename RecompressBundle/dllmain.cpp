// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <thread>
#include "il2cpp-tabledefs.h"
#include "il2cpp-class.h"
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#define DO_API(r, n, p) r (*n) p

#include "il2cpp-api-functions.h"

#undef DO_API
static HMODULE il2cpp_base = 0;
struct BuildCompression {
    uint32_t compression;
    uint32_t idk;
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
            printf("Base address of %s: %p\n", "UA", GetModuleHandleA("GameAssembly.dll"));
            return "UA";
        }

        Sleep(100);
    }
}

// Define function pointer types
//typedef void* (*RecompressAssetBundleAsync_Internal_Injected_t)(Il2CppString*, Il2CppString*, BuildCompression*, uint32_t, Priority*);
typedef void* (*RecompressAssetBundleAsync_Internal_Injected_t)(void*, void*, BuildCompression*, uint32_t, Priority*);
typedef Il2CppString* (*GetHumanReadableResult_t)(void*);
typedef bool (*GetIsDone_t)(void*);
typedef bool (*GetResult_t)(void*);
// Function pointers
RecompressAssetBundleAsync_Internal_Injected_t RecompressAssetBundleAsync_Internal_Injected = nullptr;
GetHumanReadableResult_t GetHumanReadableResult = nullptr;
GetIsDone_t GetIsDone = nullptr;
GetResult_t GetResult = nullptr;
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
std::string ConvertUtf16ToUtf8(const std::u16string& utf16Str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(utf16Str);
}
static std::wstring GetModulePath(const std::wstring& moduleName) {
    HMODULE hModule = GetModuleHandle(moduleName.c_str());
    if (!hModule) {
        std::wcerr << L"Failed to get handle for module: " << moduleName << std::endl;
        return L"";
    }

    wchar_t buffer[MAX_PATH];
    if (GetModuleFileName(hModule, buffer, MAX_PATH) == 0) {
        std::wcerr << L"Failed to get module file name for: " << moduleName << std::endl;
        return L"";
    }

    return std::wstring(buffer);
}
static std::wstring ExtractDirectory(const std::wstring& path) {
    size_t lastSlashPos = path.find_last_of(L"\\/");
    if (lastSlashPos != std::wstring::npos) {
        return path.substr(0, lastSlashPos);
    }
    return L"";
}
static std::wstring JoinPath(const std::wstring& dir, const std::wstring& filename) {
    return dir + L"\\" + filename;
}
void init_il2cpp_api() {
#define DO_API(r, n, p) n = (r (*) p)GetProcAddress(il2cpp_base, #n)
#include "il2cpp-api-functions.h"
#undef DO_API
}
std::string PrintIl2cppStringContent(void* z) {
    // Resolve function to get human readable result
    GetHumanReadableResult_t GetHumanReadableResult = (GetHumanReadableResult_t)il2cpp_resolve_icall("UnityEngine.AssetBundleRecompressOperation::get_humanReadableResult");

    // Get result from IL2CPP function
    Il2CppString* result = GetHumanReadableResult(z);

    if (result) {
        // Get pointer to UTF-16 characters
        const uint16_t* utf16Chars = il2cpp_string_chars(result);

        // Calculate length of the string (in characters)
        int length = il2cpp_string_length(result);

        // Convert UTF-16 characters to std::u16string
        std::u16string utf16String(utf16Chars, utf16Chars + length);

        // Convert UTF-16 to UTF-8
        std::string utf8String = ConvertUtf16ToUtf8(utf16String);

        // Print UTF-8 string
        if (length>0) {
            return utf8String;
        }
        else {
            return "NONE";
        }
    }
    return "NONE";
   
}
void ProcessPathsFromFile(const std::wstring& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::wcerr << L"Failed to open file: " << filePath << std::endl;
        return;
    }

    // Set locale to handle wide characters
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8_utf16<wchar_t>));
    std::string line;
    int totalLines = 0;
    while (std::getline(file, line)) {
        totalLines++;
    }
    file.clear();
    file.seekg(0, std::ios::beg);

    // Process each path
    int currentLine = 0;
    std::string path;
    while (std::getline(file, path)) {
        std::string out = path + "_recompressed"; // Example output file name

        // Build compression and priority settings
        BuildCompression buildCompression;
        buildCompression.compression = 0x00000002; // Replace with actual value
        buildCompression.idk = 0x00000005;
        buildCompression.level = 0x00000002; // Replace with actual value

        Priority priority;
        priority.priority = 0x00000004; // Replace with actual value

        // Create IL2CPP strings for path and out
        void* il2cppPath = il2cpp_string_new(path.c_str());
        void* il2cppOut = il2cpp_string_new(out.c_str());

        // Call RecompressAssetBundleAsync_Internal_Injected function
        std::cout << "Processing path (" << (currentLine + 1) << "/" << totalLines << "): " << path << std::endl;

        void* z = RecompressAssetBundleAsync_Internal_Injected(il2cppPath, il2cppOut, &buildCompression, 0, &priority);

        // Wait for the operation to complete
        while (!GetIsDone(z)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Print the content of the result
        auto humanResult = PrintIl2cppStringContent(z);
        auto succ = GetResult(z);
        std::cout << "Finished " << path << " succ: " << (succ ? "true" : "false");
        if (!humanResult.empty()) {
            std::cout << " error (NONE if succ): " << humanResult;
        }
        std::cout << std::endl;

        currentLine++;
        // Optionally add a delay between operations if needed
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    file.close();
    std::cout << " Done All!" << std::endl;
}
    //for (int i = 0; i < totalPaths; ++i) {
    //    const std::wstring& path = paths[i];
    //    std::wstring out = path + L"_recompressed"; // Example output file name

    //    // Print the current path and its index out of total
    //    std::wcout << L"Processing path (" << (i + 1) << L"/" << totalPaths << L"): " << path << std::endl;

    //    // Build compression and priority settings
    //    BuildCompression buildCompression;
    //    buildCompression.compression = 0x00000002; // Replace with actual value
    //    buildCompression.idk = 0x00000005;
    //    buildCompression.level = 0x00000002; // Replace with actual value

    //    Priority priority;
    //    priority.priority = 0x00000004; // Replace with actual value

    //    // Create IL2CPP strings for path and out
    //    void* il2cppPath = il2cpp_string_new(reinterpret_cast<const char*>(path.c_str()));
    //    void* il2cppOut = il2cpp_string_new(reinterpret_cast<const char*>(out.c_str()));

    //    // Call RecompressAssetBundleAsync_Internal_Injected function
    //    void* z = RecompressAssetBundleAsync_Internal_Injected(il2cppPath, il2cppOut, &buildCompression, 0, &priority);

    //    // Wait for the operation to complete
    //    while (!GetIsDone(z)) {
    //        //std::wcout << L"Operation for " << path << L" is still in progress...\n";
    //        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //    }
    //    auto succ = GetResult(z);

    //    // Print the content of the result
    //    PrintIl2cppStringContent(z);
    //    std::wcout << L"finished " << path << succ << std::endl;


    //    // Clean up IL2CPP strings
    ///*    il2cpp_string_free(il2cppPath);
    //    il2cpp_string_free(il2cppOut);*/

    //    // Optionally add a delay between operations if needed
    //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}

    //file.close();
//}

void ProcessAttach() {
    WaitForUA();

    HMODULE module = GetModuleHandleA("GameAssembly.dll");
    if (!module) {
        std::cerr << "Failed to get module handle for GameAssembly.dll" << std::endl;
        return;
    }

    il2cpp_base = module;

    printf("Waiting 15sec for loading game library.\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(9000));
       //printf("hi?");

    init_il2cpp_api();
    auto domain = il2cpp_domain_get();
    il2cpp_thread_attach(domain);

    // Resolve the necessary functions using il2cpp_resolve_icall
    //RecompressAssetBundleAsync_Internal_Injected = (RecompressAssetBundleAsync_Internal_Injected_t)il2cpp_resolve_icall("UnityEngine.AssetBundle::RecompressAssetBundleAsync_Internal_Injected");
    GetHumanReadableResult = (GetHumanReadableResult_t)il2cpp_resolve_icall("UnityEngine.AssetBundleRecompressOperation::get_humanReadableResult");
    GetIsDone = (GetIsDone_t)il2cpp_resolve_icall("UnityEngine.AsyncOperation::get_isDone");
    GetResult = (GetResult_t)il2cpp_resolve_icall("UnityEngine.AssetBundleRecompressOperation::get_success");
    RecompressAssetBundleAsync_Internal_Injected = (RecompressAssetBundleAsync_Internal_Injected_t)il2cpp_resolve_icall("UnityEngine.AssetBundle::RecompressAssetBundleAsync_Internal_Injected");

    if (!RecompressAssetBundleAsync_Internal_Injected || !GetHumanReadableResult) {
        std::cerr << "Failed to resolve functions" << std::endl;
        return;
    }
    auto path = GetModulePath(L"RecompressBundle.dll");
    ProcessPathsFromFile(JoinPath(ExtractDirectory(path), L"filelist.txt"));

    //std::string path ="D:/000faf16fe62cc7a45e28f242e6a5c01.dab";
    //std::string out = "D:/000faf16fe62cc7a45e28f242e6a5c01.tab";
    //BuildCompression buildCompression;
    //buildCompression.compression = 0x0000002; // Replace with actual value
    //buildCompression.idk = 0x00000005;
    //buildCompression.level = 0x00000002; // Replace with actual value

    //Priority priority;
    //priority.priority = 00000004; // Replace with actual value

    //// Create Il2Cpp strings
    ////Il2CppString* il2cppPath = CreateIl2CppString(path);
    ////Il2CppString* il2cppOut = CreateIl2CppString(out);
    ////Sleep(5);
    //void* il2cppPath = il2cpp_string_new(path.c_str());
    //void* il2cppOut = il2cpp_string_new(out.c_str());
    //// Call the RecompressAssetBundleAsync function

    //void* z = RecompressAssetBundleAsync_Internal_Injected(il2cppPath, il2cppOut, &buildCompression, 0, &priority);
    //// Check if the operation is done
    //while (!GetIsDone(z)) {
    //    printf("Operation is still in progress...\n");
    //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}
    //PrintIl2cppStringContent(z);
    //auto result = GetHumanReadableResult(z);
    ////const char* stringChars = reinterpret_cast<const char*>(il2cpp_string_chars(result));
    //const uint16_t* utf16Chars = il2cpp_string_chars(result);
    //int length = il2cpp_string_length(result);
    //// Convert UTF-16 characters to std::u16string
    //std::u16string utf16String(utf16Chars, utf16Chars + length);
    //std::string utf8String = ConvertUtf16ToUtf8(utf16String);
    //printf("String content: %s\n", utf8String.c_str());

    ////printf("test %s \n", il2cpp_string_chars(result));
    //printf("aaaaaaaaa %p \n",z);
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
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessAttach, new HMODULE(hModule), 0, NULL);
        //printf("WTQWRWEQ");
        //WaitForDebugger();
        //ProcessAttach();
        //printf("Waiting 15sec for loading game library.");
        //Sleep(9);
        //printf("hi?");

        //std::thread(ProcessAttach).detach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
