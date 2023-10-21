#include "download.h"
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <wininet.h>
#include "console.h"
#include <Shlobj.h>
#include <Shlwapi.h>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>

bool directoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    else if (info.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}


void ShowProgressBar(double percent, double downloadedMB, double totalMB) {
    const int barWidth = 50;
    std::cout << "[";

    for (int i = 0; i < barWidth; ++i) {
        if (i < static_cast<int>(barWidth * percent / 100.0)) {
            std::cout << "=";
        }
        else {
            std::cout << " ";
        }
    }

    std::cout << "] " << std::fixed << std::setprecision(2) << percent << "% ";
    std::cout << downloadedMB << "MB/" << totalMB << "MB\r";
    std::cout.flush();
}

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

    DWORD fileSize = 0;
    DWORD dwSize = sizeof(fileSize);
    if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &fileSize, &dwSize, NULL)) {
        ErrorMessage("Failed to get file size.");
    }

    DWORD totalBytesRead = 0;
    double totalMB = fileSize / (1024.0 * 1024.0);

    if (!CreateDirectoryA(destinationDir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ErrorMessage("Failed to create the installation directory ");
        return false;
    }

    std::ofstream outputFile(destinationPath, std::ios::binary);
    if (!outputFile) {
        ErrorMessage("Failed to create or open the destination file.");
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    SuccessMessage("Created Fluster installation path: " + destinationDir);
    SuccessMessage("Starting to Download Fluster Binary... ");

    char buffer[4096];
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        outputFile.write(buffer, bytesRead);

        totalBytesRead += bytesRead;
        double percent = (static_cast<double>(totalBytesRead) / fileSize) * 100.0;
        double downloadedMB = totalBytesRead / (1024.0 * 1024.0);
        ShowProgressBar(percent, downloadedMB, totalMB);
    }

    std::cout << std::endl;
    outputFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    SuccessMessage("Downloaded the Fluster Binary in " + destinationPath);

    RenameAndExtractToFolder(destinationPath, destinationDir);

    return true;
}



bool AddAppxPackage(const std::string& appxManifestPath) {

    std::wstring addAppxCommand = L"powershell -Command \"Add-AppxPackage -pat\h '" + std::wstring(appxManifestPath.begin(), appxManifestPath.end()) + L"' -register\"";

    HINSTANCE result = ShellExecuteW(nullptr, L"open", L"powershell.exe", addAppxCommand.c_str(), nullptr, SW_HIDE);

    if ((int)result > 32) {
        WarningMessage("Waiting for package data...");
        while (!directoryExists("C:\\Fluster\\microsoft.system.package.metadata")) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        SuccessMessage("Installed Fluster, Type Fluster in your application bar");
        return true;
    }
    else {
        ErrorMessage("Failed to install Fluster Something went wrong woomp woomp");
        return false;
    }
}

bool ExtractZipFile(const std::string& zipFilePath, const std::string& extractPath) {
    std::wstring expandCommand = L"Expand-Archive \"" + std::wstring(zipFilePath.begin(), zipFilePath.end()) +
        L"\" -DestinationPath \"" + std::wstring(extractPath.begin(), extractPath.end()) + L"\"";

    std::wstring commandLine = L"powershell.exe " + expandCommand;

    wchar_t* commandLineNonConst = new wchar_t[commandLine.length() + 1];
    wcscpy_s(commandLineNonConst, commandLine.length() + 1, commandLine.c_str());

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};

    if (CreateProcessW(NULL, commandLineNonConst, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        SuccessMessage("Extracting Fluster Binary. Please wait...");
        WarningMessage("This may take a while depending on your SSD/Hard drive.");

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (DeleteFileW(std::wstring(zipFilePath.begin(), zipFilePath.end()).c_str())) {
            delete[] commandLineNonConst;
            
            AddAppxPackage(extractPath + "\\AppxManifest.xml");
        }
        else {
            delete[] commandLineNonConst;

            return false;
        }
    }
    else {
        ErrorMessage("Failed to extract Fluster's binary");

        delete[] commandLineNonConst;

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


