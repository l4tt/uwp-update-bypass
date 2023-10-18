#include "download.h"
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <wininet.h>
#include "console.h"
#include <Shlobj.h>
#include <Shlwapi.h>



bool DownloadFileToCDrive(const std::string& url, const std::string& destinationDir) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    std::string fileName = url.substr(url.find_last_of("/") + 1);
    std::string destinationPath = destinationDir + "\\" + fileName;
    
    hInternet = InternetOpen(L"DownloadFileToCDrive", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
    if (hInternet == nullptr) {
        ErrorMessage("Failed to Download File ");
        return false;
    }

    hConnect = InternetOpenUrlA(hInternet, url.c_str(), nullptr, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == nullptr) {
        ErrorMessage("Failed to Download File ");
        InternetCloseHandle(hInternet);
        return false;
    }
    if (!CreateDirectoryA(destinationDir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ErrorMessage("Looks like you already have fluster installed please delete the path: ");
        return false;
    }

    std::ofstream outputFile(destinationPath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Failed to create or open the destination file." << std::endl;
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    SuccessMessage("Created Fluter install path: " + destinationDir);
   
    SuccessMessage("Starting to Download Fluter Binary... ");

    char buffer[4096];
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        outputFile.write(buffer, bytesRead);
    }

    outputFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    SuccessMessage("Downloaded the Fluter Binary in " + destinationPath);


    RenameAndExtractToFolder(destinationPath, destinationDir);
    
    return true;
}


bool AddAppxPackage(const std::string& appxManifestPath) {

    std::wstring addAppxCommand = L"powershell -Command \"Add-AppxPackage -path '" + std::wstring(appxManifestPath.begin(), appxManifestPath.end()) + L"' -register\"";

    HINSTANCE result = ShellExecuteW(nullptr, L"open", L"powershell.exe", addAppxCommand.c_str(), nullptr, SW_HIDE);

    if ((int)result > 32) {
        SuccessMessage("Installed Fluster, Type Fluster in your application bar");
        return true;
    }
    else {
        ErrorMessage("Failed to install Fluster Something went wrong woomp woomp");
        return false;
    }
}



bool ExtractZipFile(const std::string& zipFilePath, const std::string& extractPath) {
    
    std::wstring expandCommand = L"Expand-Archive \"" + std::wstring(zipFilePath.begin(), zipFilePath.end()) + L"\" -DestinationPath \"" + std::wstring(extractPath.begin(), extractPath.end()) + L"\"";


    HINSTANCE result = ShellExecuteW(nullptr, L"open", L"powershell.exe", expandCommand.c_str(), nullptr, SW_HIDE);

    if ((int)result > 32) {
        SuccessMessage("Extracting Fluster Binary Please wait 90s");
        WarningMessage("This May take awhile depending on your SSD/Hardrive");

        Sleep(90000);
        AddAppxPackage(extractPath + "\\AppxManifest.xml");
        return true;
    }
    else {
        ErrorMessage("Failed to Extract Flusters Binary");
        return false;
    }
}





bool RenameAndExtractToFolder(const std::string& sourceFilePath, const std::string& destinationFolder) {

    std::string renamedFilePath = sourceFilePath + ".zip";
    if (!MoveFileA(sourceFilePath.c_str(), renamedFilePath.c_str())) {
        ErrorMessage("Failed to rename File");
        return false;
    }


    if (!CreateDirectoryA(destinationFolder.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ErrorMessage("Failed to Create path");
        return false;
    }

    SuccessMessage("Renamed Fluster Binary for extraction");

    ExtractZipFile(renamedFilePath, destinationFolder);
}


