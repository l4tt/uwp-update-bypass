#include <iostream>
#include <Windows.h>
#include <Shellapi.h>
#include <VersionHelpers.h>
#include "console.h";
#include "installer.h"

bool SetDeveloperMode(bool enable)
{

    if (IsWindowsVersionOrGreater(10, 0, 0)) {
        // Windows 10

        if (enable == TRUE) {
            WarningMessage("You are on Windows 10, please make sure you enabled developer mode!");
            system("pause");
            return true;
        }
        else {
            return true;
        }
    }

    const wchar_t* command;


    if (enable)
    {
        command = L"powershell -Command if ((Get-ItemPropertyValue -Path 'HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock' -Name 'AllowDevelopmentWithoutDevLicense') -eq 1) { Write-Output 'Developer Mode is already enabled.' } else { Set-ItemProperty -Path 'HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock' -Name 'AllowDevelopmentWithoutDevLicense' -Value 1; }";
    }
    else
    {
        command = L"powershell -Command if ((Get-ItemPropertyValue -Path 'HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock' -Name 'AllowDevelopmentWithoutDevLicense') -eq 0) { Write-Output 'Developer Mode is already disabled.' } else { Set-ItemProperty -Path 'HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock' -Name 'AllowDevelopmentWithoutDevLicense' -Value 0; }";
    }

    DWORD resultCode = 0;
    BOOL result = RunCommand(command, resultCode);
    if (result)
    {
        if (resultCode != 0) {
            return false;
        }
        else {
            return true;
        }
    }
    return false;
}

std::pair<bool, bool> IsFlusterInstalled(const std::string& dirPath, const std::string& fileName) {
    bool dirExists = false;
    bool fileExists = false;

    DWORD attribs = GetFileAttributesA(dirPath.c_str());
    if (attribs != INVALID_FILE_ATTRIBUTES) {
        dirExists = (attribs & FILE_ATTRIBUTE_DIRECTORY);
    }

    std::string fullPath = dirPath + "\\" + fileName;
    attribs = GetFileAttributesA(fullPath.c_str());
    if (attribs != INVALID_FILE_ATTRIBUTES) {
        fileExists = true;
    }

    return { dirExists, fileExists };
}

bool DeleteDirectoryRecursively(const std::string& path) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }
    do {
        const std::string fileOrDir = path + "\\" + findFileData.cFileName;
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, ".") != 0 &&
                strcmp(findFileData.cFileName, "..") != 0) {
                DeleteDirectoryRecursively(fileOrDir);
            }
        }
        else {
            DeleteFileA(fileOrDir.c_str());
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);

    return RemoveDirectoryA(path.c_str());
}

bool IsProcessRunningAsAdmin()
{
    BOOL isRunAsAdmin = FALSE;
    PSID adminGroupSid = nullptr;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &adminGroupSid))
    {
        return false;
    }

    if (!CheckTokenMembership(NULL, adminGroupSid, &isRunAsAdmin))
    {
        isRunAsAdmin = FALSE;
    }

    if (adminGroupSid)
    {
        FreeSid(adminGroupSid);
    }

    return isRunAsAdmin;
}

int main()
{
    IntroMessage();
    WarningMessage("Installer made by Nano and edited by Pixeluted - discord.gg/runesoftware");
    WarningMessage("Big thanks to cereal for making uwp update bypasser!\n\n");

    ErrorMessage("Fluster is patched as of right now! Please wait until it gets unpatched, if you want to get notification join discord server discord.gg/runesoftware");
    system("pause");
    return 0;

    if (!IsProcessRunningAsAdmin()) {
        ErrorMessage("Please run this installer as administrator!");
        system("pause");
        return 0;
    }

    std::string url = "https://github.com/cerealwithmilk/uwp/releases/download/test/Fluster.msix";
    std::string fileName = "Windows10Universal.exe";
    std::string destinationDir = "C:\\Fluster";

    std::pair<bool, bool> flusterCheck = IsFlusterInstalled(destinationDir, fileName);

    if (flusterCheck.first) {
        // Fluster directory exists
        if (flusterCheck.second) {
            // Fluster is installed!
            WarningMessage("Fluster is already installed! If you want to reinstall, delete the Fluster folder in C drive ");
            system("pause");
            return 0;
        }
        else {
            // Corruputed installation
            if (DeleteDirectoryRecursively(destinationDir.c_str())) {
                WarningMessage("Fluster is installed but corrupted, Fluster will be re-installed ");
            }
            else {
                WarningMessage("Failed to delete the Fluster directory to start re-installation ");
                system("pause");
                return 0;
            }
        }
    }

    if (!SetDeveloperMode(true))
    {
        ErrorMessage("Failed to enable Developer Mode.");
        system("pause");
        return 0;

    }

    SuccessMessage("Sucessfully enabled developer mode");

    if (DownloadFileToCDrive(url, destinationDir)) {
        SuccessMessage("Thank you for using Fluster - Rune on top / Byfron == Bad");
    }
    else {
        ErrorMessage("Failed to Download Fluster");
    }

    if (!SetDeveloperMode(false)) {
        ErrorMessage("Failed to disable Developer Mode.");
    }
    else {
        SuccessMessage("Sucessfully disabled developer mode");
    }


    system("pause");

    return 0;
}