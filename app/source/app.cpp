#include "database.h"
#include <iostream>
#include <iomanip>
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

void display_menu() {
    std::cout << "\n=== Finance Tracker ===" << std::endl;
    std::cout << "1. Add Expense" << std::endl;
    std::cout << "2. Add Income" << std::endl;
    std::cout << "3. Add Account State" << std::endl;
    std::cout << "4. Edit Entry" << std::endl;
    std::cout << "5. View Month Summary" << std::endl;
    std::cout << "6. View All Entries" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "\nChoice: ";
}

void display_edit_entry_menu(){
    std::cout << "\n=== Editing Entry ===" << std::endl;
    std::cout << "1. Expense" << std::endl;
    std::cout << "2. Income" << std::endl;
    std::cout << "3. Account State" << std::endl;
    std::cout << "4. Name" << std::endl;
    std::cout << "5. Value" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "\nChoice: ";
}

void add_entry(finance::Database& db, const std::string& month, const std::string& type) {
    std::string name;
    double value;
    
    std::cout << "\nEnter " << type << " name: ";
    std::getline(std::cin, name);
    
    std::cout << "Enter amount: ";
    std::cin >> value;
    std::cin.ignore(); // Clear newline
    
    if (value <= 0) {
        std::cout << "Amount must be positive!" << std::endl;
        return;
    }
    
    if (db.add_entry(month, type, name, value)) {
        std::cout << "✓ " << type << " added successfully!" << std::endl;
    } else {
        std::cout << "✗ Failed to add " << type << std::endl;
    }
}

void edit_entry(int id, 
                const std::string& new_type = "None", 
                const std::string& new_name = "None", 
                double new_value = 0.0) {
}

void view_summary(finance::Database& db, const std::string& month) {
    double total_income = db.get_total_income(month);
    double total_expenses = db.get_total_expenses(month);
    double balance = total_income - total_expenses;
    
    std::cout << "\n=== Summary for " << month.substr(0, 7) << " ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Income:   $" << total_income << std::endl;
    std::cout << "Total Expenses: $" << total_expenses << std::endl;
    std::cout << "Balance:        $" << balance;
    
    if (balance >= 0) {
        std::cout << " ✓" << std::endl;
    } else {
        std::cout << " ✗" << std::endl;
    }
}

void view_entries(finance::Database& db, const std::string& month) {
    auto entries = db.get_entries_by_month(month);
    
    if (entries.empty()) {
        std::cout << "\nNo entries found for this month." << std::endl;
        return;
    }
    
    std::cout << "\n=== Entries for " << month.substr(0, 7) << " ===" << std::endl;
    std::cout << std::left << std::setw(10) << "ID" 
              << std::setw(30) << std::setw(10) << "Type" 
              << std::setw(30) << "Name" 
              << std::right << std::setw(12) << "Amount" << std::endl;
    std::cout << std::string(52, '-') << std::endl;
    
    for (const auto& entry : entries) {
        std::cout << std::left << std::setw(10) << entry.id
                  << std::setw(30) << std::setw(10) << entry.type
                  << std::setw(30) << entry.name
                  << std::right << std::setw(12) << std::fixed << std::setprecision(2) 
                  << "$" << entry.value << std::endl;
    }
}

int main() {
    // Database connection string
    // Format: "host=localhost port=5432 dbname=finances user=youruser password=yourpass"
    const char* conn_str = std::getenv("DB_CONNECTION_STRING");
    
    if (!conn_str) {
        std::cerr << "Error: DB_CONNECTION_STRING environment variable not set" << std::endl;
        std::cerr << "Example: export DB_CONNECTION_STRING='host=localhost dbname=finances user=postgres password=yourpass'" << std::endl;
        return 1;
    }
    
    try {
        finance::Database db(conn_str);
        db.initialize();
        
        std::string current_month = get_month_input();
        
        while (true) {
            display_menu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            switch (choice) {
                case 1:
                    add_entry(db, current_month, "expense");
                    break;
                case 2:
                    add_entry(db, current_month, "income");
                    break;
                case 3:
                    add_entry(db, current_month, "account_state");
                    break;
                case 4:{
                    std::cout << "Enter the entry id to be edited: " << std::endl;
                    int id;
                    std::cin >> id;
                    std::cin.ignore();
                    display_edit_entry_menu();
                    int inner_choice;
                    std::cin >> inner_choice;
                    std::cin.ignore();
                    switch (inner_choice){
                        case 1:
                            edit_entry(id, "expense");
                            break;
                        case 2:
                            edit_entry(id, "income");
                            break;
                        case 3:
                            edit_entry(id, "account_state");
                            break;
                        case 4: {
                            std::string name;
                            std::cout << "Enter new name: " << std::endl;
                            std::cin >> name;
                            std::cin.ignore();
                            edit_entry(id, "None", name);  // Only change name
                            break;
                            }
                        case 5:{
                            double value;
                            std::cout << "Enter new value: " << std::endl;
                            std::cin >> value;
                            std::cin.ignore();
                            edit_entry(id, "None", "None", value);  // Only change value
                            break;
                               }
                        case 6:
                            break;
                    }
                       }

                case 5:
                    view_summary(db, current_month);
                    break;
                case 6:
                    view_entries(db, current_month);
                    break;
                case 8:
                    std::cout << "Goodbye!" << std::endl;
                    return 0;
                default:
                    std::cout << "Invalid choice!" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
