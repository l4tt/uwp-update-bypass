#include "console.h";

void SuccessMessage(std::string Description) {
	std::cout << "[ " << termcolor::green << "Success" << termcolor::reset << " ] - " << Description << std::endl;
}

void ErrorMessage(std::string Description) {
	std::cout << "[ " << termcolor::red << "Error" << termcolor::reset << " ] - " << Description << std::endl;
}

void WarningMessage(std::string Description) {
	std::cout << "[ " << termcolor::bright_yellow << "Warning" << termcolor::reset << " ] - " << Description << std::endl;
}

void IntroMessage() {
	std::string Intro = R"(
	_______        _     _ _______ _______ _______  ______  
	|______ |      |     | |______    |    |______ |_____/   
	|       |_____ |_____| ______|    |    |______ |    \_  
)";
	std::string Version = "\n		  Version: 1.5 ";
	std::string RobloxVersion = " Roblox: 2.598.613";


	std::cout << termcolor::blue << Intro << termcolor::yellow << Version << termcolor::green << RobloxVersion << termcolor::reset << "\n\n";
	
}