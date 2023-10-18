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

int main()
{
    IntroMessage();
    WarningMessage("Installer made by Nano - discord.gg/runesoftware");
    WarningMessage("Big thanks to cereal for making uwp update bypasser!\n\n");

    if (!EnableDeveloperMode())
    {
        ErrorMessage("Failed to enable Developer Mode.");
        
    }

    SuccessMessage("Enabled Microsoft Developer Setting Enabled");


    std::string url = "https://github.com/cerealwithmilk/uwp/releases/download/test/Fluster.msix";
    std::string destinationDir = "C:\\Fluster";

    if (DownloadFileToCDrive(url, destinationDir)) {
        SuccessMessage("Thank you for using Fluster - Rune on top / Byfron == Bad");
    }
    else {
        ErrorMessage("Failed........ to Download Fluster");
    }
    return 0;
}