#pragma once
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

bool DownloadFileToCDrive(const std::string& url, const std::string& destinationDir);
bool RenameAndExtractToFolder(const std::string& sourceFilePath, const std::string& destinationFolder);
bool RunCommand(const std::wstring& cmd, DWORD& exitCode);