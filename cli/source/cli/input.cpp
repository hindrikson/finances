#include "database/database.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <regex>
#include <cstdlib>

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

std::string get_month_input() {
    std::string input;
    std::regex month_pattern(R"(^\d{4}-(0[1-9]|1[0-2])$)");
    
    while (true) {
        std::cout << "\nEnter month (YYYY-MM): ";
        std::getline(std::cin, input);
        
        if (std::regex_match(input, month_pattern)) {
            return input + "-01"; // Add day to make it a valid date
        }
        
        std::cout << "Invalid format. Please use YYYY-MM (e.g., 2024-11)" << std::endl;
    }
}
