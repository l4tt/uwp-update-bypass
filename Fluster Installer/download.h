#pragma once

#include <string>

bool DownloadFileToCDrive(const std::string& url, const std::string& destinationDir);
bool RenameAndExtractToFolder(const std::string& sourceFilePath, const std::string& destinationFolder);