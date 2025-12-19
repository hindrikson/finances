#include "database/database.h"
#include "cli/display.h"
#include "cli/input.h"
#include <iostream>

namespace cli_handlers {

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

    void delete_entry(finance::Database& db,
            int id
            ){

        db.delete_entry(id);

        std::cout << "Entry " << id << " was successfully deleted." << std::endl;

    }

    void edit_entry(finance::Database& db,
            int id,
            std::optional<std::string> type = std::nullopt,
            std::optional<std::string> name = std::nullopt,
            std::optional<double> value = std::nullopt
            ){

        if (type.has_value()){
            db.update_type(id, type.value());
        }

        if (name.has_value()){
            db.update_name(id, name.value());
        }

        if (value.has_value()){
            db.update_value(id, value.value());
        }
    }

    void handle_edit_entry(finance::Database& db, std::string& month){
        int id = input::get_entry_id();

        if (!db.entry_exists(id, month)){
            std::cout << "Error: No entry found with ID " << id << std::endl;
            return;
        }

        cli::display_edit_entry_menu(db, id);

        int choice;

        if (!(std::cin >> choice)){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
            std::cout << "Invalid choice" << std::endl;
            return;
        }
        
        switch (choice) {
            case 1:
                edit_entry(db, id, "expense");
                break;

            case 2:
                edit_entry(db, id, "income");
                break;

            case 3:
                edit_entry(db, id, "account_state");
                break;

            case 4: {
                        std::string name;
                        std::cout << "Enter new name: ";
                        std::getline(std::cin, name);

                        if (name.empty()){
                            std::cout <<"Name can not be empty!" << std::endl;
                            return;
                        }

                        edit_entry(db, id, std::nullopt, name);
                        break;
                    }
            case 5: {
                        double value;
                        std::cout << "Enter new value: ";

                        if (!(std::cin >> value)){
                            std::cin.clear();
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cout << "Ivalid value!" << std::endl;
                            return;
                        }

                        std::cin.ignore(); // clear new line
                        
                        edit_entry(db, id, std::nullopt, std::nullopt, value);
                        break;
                    }
            case 6: 
                cli_handlers::delete_entry(db, id);
                break;
            case 7: 
                std::cout << "Edit cancelled." << std::endl;
                break;

            default:
                std::cout << "Invalid choice!" << std::endl;
        }

    }
}

