#include "input.h" 
#include <iostream>
#include <limits>
#include <regex>

namespace input {

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

    int get_entry_id(){
        int id;
        while (true){
            std::cout << "Enter the entry id to be edited: ";
            if(std::cin >> id){
                std::cin.ignore();
                return id;
            }
            // invalid input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid ID. Please enter a valid integer." << std::endl;
        }
    }

}
