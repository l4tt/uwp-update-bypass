#include <iostream>
#include <Windows.h>
#include <Shellapi.h>
#include <VersionHelpers.h>
#include "console.h";
#include "installer.h"

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

    WarningMessage("Fluster requires the UWP Update Required prompt bypasser, please make sure to use the fiddler bypass.");

    if (!IsProcessRunningAsAdmin()) {
        ErrorMessage("Please run this installer as administrator!");
        system("pause");
        return 0;
    }

    std::string url = "https://github.com/l4tt/uwp-update-bypass/releases/download/1.5/Fluster.msix";
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

    if (DownloadFileToCDrive(url, destinationDir)) {
        SuccessMessage("Thank you for using Fluster - Rune on top / Byfron == Bad");
    }
    else {
        ErrorMessage("Failed to Download Fluster");
    }

    system("pause");

    return 0;
}
