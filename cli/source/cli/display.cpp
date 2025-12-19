#include "display.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

namespace cli {

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

    void display_edit_entry_menu(finance::Database& db, int id){

        std::cout << "\n=== Editing Entry ===" << std::endl;

        auto entry = db.entry_info(id);
        if (entry.empty()) {
            std::cout << "\nNo information found for this entry." << std::endl;
            return;
        }
        std::cout << "\nEntry Info" << " ===" << std::endl;
        std::cout << std::left << std::setw(10) << "ID" 
                  << std::setw(15) << "Month" 
                  << std::setw(10) << "Type" 
                  << std::setw(30) << "Name" 
                  << std::right << std::setw(12) << "Amount" << std::endl;
        std::cout << std::string(72, '-') << std::endl;

        for (const auto& item : entry) {
            std::cout << std::left << std::setw(10) << item.id
                      << std::setw(15) << item.month
                      << std::setw(10) << item.type
                      << std::setw(30) << item.name
                      << std::right << std::setw(7) << std::fixed << std::setprecision(2) 
                      << "$" << item.value << std::endl;
        }

        std::cout << "" << std::endl;
        std::cout << "1. Expense" << std::endl;
        std::cout << "2. Income" << std::endl;
        std::cout << "3. Account State" << std::endl;
        std::cout << "4. Name" << std::endl;
        std::cout << "5. Value" << std::endl;
        std::cout << "6. Delete entry" << std::endl;
        std::cout << "7. Exit" << std::endl;
        std::cout << "\nChoice: ";
    }

    void clear_screen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
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
}
