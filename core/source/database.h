#pragma once
#include <string>
#include <vector>
#include <memory>
#include <pqxx/pqxx>

namespace finance {

struct Entry {
    int id;
    std::string month;      // Format: YYYY-MM-01
    std::string type;       // "expense" or "income"
    std::string name;
    double value;
    std::string created_at;
};

class Database {
public:
    Database(const std::string& connection_string);
    ~Database();

    // Initialize database schema
    void initialize();

    // Add an entry
    bool add_entry(const std::string& month, const std::string& type, 
                   const std::string& name, double value);

    // Edit entry
    bool edit_entry(int id, const std::string& new_type = "None", const std::string& new_name = "None", double new_value = 0.0);

    // Get all entries for a specific month
    std::vector<Entry> get_entries_by_month(const std::string& month);

    // Get summary for a month
    double get_total_income(const std::string& month);
    double get_total_expenses(const std::string& month);

private:
    std::unique_ptr<pqxx::connection> conn_;
};

} // namespace finance
