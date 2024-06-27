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
#include <filesystem>  

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
    wchar_t chars[1]; 
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

typedef void* (*RecompressAssetBundleAsync_Internal_Injected_t)(void*, void*, BuildCompression*, uint32_t, Priority*);
typedef Il2CppString* (*GetHumanReadableResult_t)(void*);
typedef bool (*GetIsDone_t)(void*);
typedef bool (*GetResult_t)(void*);
RecompressAssetBundleAsync_Internal_Injected_t RecompressAssetBundleAsync_Internal_Injected = nullptr;
GetHumanReadableResult_t GetHumanReadableResult = nullptr;
GetIsDone_t GetIsDone = nullptr;
GetResult_t GetResult = nullptr;
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
static std::string GetModulePath(const std::string& moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    if (!hModule) {
        std::cerr << "Failed to get handle for module: " << moduleName << std::endl;
        return "";
    }

    char buffer[MAX_PATH];
    if (GetModuleFileNameA(hModule, buffer, MAX_PATH) == 0) {
        std::cerr << "Failed to get module file name for: " << moduleName << std::endl;
        return "";
    }

    return std::string(buffer);
}

static std::wstring ExtractDirectory(const std::wstring& path) {
    size_t lastSlashPos = path.find_last_of(L"\\/");
    if (lastSlashPos != std::wstring::npos) {
        return path.substr(0, lastSlashPos);
    }
    return L"";
}
std::string ExtractDirectory(const std::string& filePath) {
    size_t found = filePath.find_last_of("/\\");
    return filePath.substr(0, found);
}
std::string ExtractFilename(const std::string& filePath) {
    size_t found = filePath.find_last_of("/\\");
    return filePath.substr(found + 1);
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
    GetHumanReadableResult_t GetHumanReadableResult = (GetHumanReadableResult_t)il2cpp_resolve_icall("UnityEngine.AssetBundleRecompressOperation::get_humanReadableResult");

    Il2CppString* result = GetHumanReadableResult(z);

    if (result) {
        const uint16_t* utf16Chars = il2cpp_string_chars(result);

        int length = il2cpp_string_length(result);

        std::u16string utf16String(utf16Chars, utf16Chars + length);

        std::string utf8String = ConvertUtf16ToUtf8(utf16String);

        if (length>0) {
            return utf8String;
        }
        else {
            return "NONE";
        }
    }
    return "NONE";
   
}
bool CreateDirectories(const std::string& path) {
    if (!CreateDirectoryA(path.c_str(), NULL)) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            return true; 
        }
        else {
            std::cerr << "Failed to create directory: " << path << std::endl;
            return false; 
        }
    }
    return true; 
}
void ProcessPathsFromFile(const std::wstring& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::wcerr << L"Failed to open file: " << filePath << std::endl;
        return;
    }

    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8_utf16<wchar_t>));
    std::string line;
    int totalLines = 0;
    while (std::getline(file, line)) {
        totalLines++;
    }
    file.clear();
    file.seekg(0, std::ios::beg);

    int currentLine = 0;
    std::string path;
    std::string out = ExtractDirectory(GetModulePath("RecompressBundle.dll")) + "\\recompress\\";
    if (!CreateDirectories(out)) {
        std::cerr << "Failed to create output directory: " << out << std::endl;
        return;
    }
    while (std::getline(file, path)) {
        std::string fileName = ExtractFilename(path);
        std::string outputPath = out + fileName;
        BuildCompression buildCompression;
        buildCompression.compression = 0x00000000; //0x0000000{compress}//LZ4 2 LZ4HC 3 Uncompressed 0
        buildCompression.idk = 0x00000005;
        buildCompression.level = 0x00000002; 

        Priority priority;
        priority.priority = 0x00000004; 

        void* il2cppPath = il2cpp_string_new(path.c_str());
        void* il2cppOut = il2cpp_string_new(outputPath.c_str());

        std::cout << "Processing path (" << (currentLine + 1) << "/" << totalLines << "): " << path << std::endl;

        void* z = RecompressAssetBundleAsync_Internal_Injected(il2cppPath, il2cppOut, &buildCompression, 0, &priority);

       /* while (!GetIsDone(z)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }*/

        //auto humanResult = PrintIl2cppStringContent(z);
        //auto succ = GetResult(z);
        std::cout << "Finished " << path << std::endl;

        /*std::cout << "Finished " << path << " succ: " << (succ ? "true" : "false");
        if (humanResult != "NONE") {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (!humanResult.empty()) {
            std::cout << " error (NONE if succ): " << humanResult;
        }
        std::cout << std::endl;*/
        currentLine++;
    }
    file.close();
    std::cout << " Done All!" << std::endl;
    std::cout << " Now Wait till items in folder matches total count " << totalLines<<std::endl;
}

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

    init_il2cpp_api();
    auto domain = il2cpp_domain_get();
    il2cpp_thread_attach(domain);

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
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        AttachConsole();
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessAttach, new HMODULE(hModule), 0, NULL);

        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
