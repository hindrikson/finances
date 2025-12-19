#include "database/database.h"
#include "cli/display.h"
#include "cli/input.h"
#include "cli/handlers.h"
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
        
        std::string current_month = input::get_month_input();
        
        while (true) {
            cli::display_menu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            switch (choice) {
                case 1:
                    cli_handlers::add_entry(db, current_month, "expense");
                    break;
                case 2:
                    cli_handlers::add_entry(db, current_month, "income");
                    break;
                case 3:
                    cli_handlers::add_entry(db, current_month, "account_state");
                    break;
                case 4:
                    cli_handlers::handle_edit_entry(db, current_month);
                      break;
                case 5:
                      cli::view_summary(db, current_month);
                    break;
                case 6:
                    cli::view_entries(db, current_month);
                    break;
                case 7:
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
