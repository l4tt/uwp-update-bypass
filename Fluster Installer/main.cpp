#include <iostream>
#include <Windows.h>
#include <Shellapi.h>
#include "console.h";
#include "download.h";

bool EnableDeveloperMode()
{
    const wchar_t* command = L"-Command if ((Get-ItemPropertyValue -Path 'HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock' -Name 'AllowDevelopmentWithoutDevLicense') -eq 1) { Write-Output 'Developer Mode is already enabled.' } else { Set-ItemProperty -Path 'HKLM:\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock' -Name 'AllowDevelopmentWithoutDevLicense' -Value 1; }";

    HINSTANCE result = ShellExecuteW(nullptr, L"open", L"powershell.exe", command, nullptr, SW_HIDE);
    if ((int)result > 32)
    {
        return true;
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

int main()
{
    IntroMessage();
    WarningMessage("Installer made by Nano - discord.gg/runesoftware");
    WarningMessage("Big thanks to cereal for making uwp update bypasser!\n\n");

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

    if (!EnableDeveloperMode())
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

    system("pause");

    return 0;
}