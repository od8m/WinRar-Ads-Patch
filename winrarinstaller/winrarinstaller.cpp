#include <windows.h>
#include <shellapi.h>
#include <fstream>
#include "resource1.h"

bool IsElevated() {
    HANDLE hToken;
    BOOL fRet = FALSE;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize)) {
            fRet = elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return fRet;
}

void RequestAdminPrivileges() {
    if (!IsElevated()) {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = L"WinRAR.exe";
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        ShellExecuteEx(&sei);
        exit(1);
    }
}

bool ExtractResource(LPCWSTR resourceName, const char* outputFileName) {
    HRSRC hRes = FindResource(NULL, resourceName, RT_RCDATA);
    if (!hRes) return false;

    HGLOBAL hResData = LoadResource(NULL, hRes);
    if (!hResData) return false;

    DWORD dataSize = SizeofResource(NULL, hRes);
    void* data = LockResource(hResData);
    if (!data) return false;

    std::ofstream outFile(outputFileName, std::ios::binary);
    if (!outFile) return false;

    outFile.write((char*)data, dataSize);
    outFile.close();
    return true;
}

void ReplaceWinRAR() {
    char sourceFile[] = "temp.exe";
    char destFile[] = "C:\\Program Files\\WinRAR\\WinRar.exe";

    if (!ExtractResource(MAKEINTRESOURCE(IDR_WINRAREXE), sourceFile)) {
        return;
    }

    int result = MessageBoxA(NULL, "Replace WinRAR?", "Confirm", MB_YESNO | MB_ICONQUESTION);
    if (result == IDYES) {
        if (CopyFileA(sourceFile, destFile, FALSE)) {
            MessageBoxA(NULL, "Welcome! Enjoy free software :)", "Success", MB_OK | MB_ICONINFORMATION);
        }
    }

    DeleteFileA(sourceFile);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MessageBoxA(NULL, "Hello from elevated task!", "Info", MB_OK | MB_ICONINFORMATION);
    RequestAdminPrivileges();
    ReplaceWinRAR();
    return 0;
}