#include "database.h"
#include <iostream>
#include <sstream>

namespace finance {

// In database.cpp
Database::Database(const std::string& connection_string) {
    std::cout << "Database constructor called" << std::endl;
    try {
        conn_ = std::make_unique<pqxx::connection>(connection_string);
        if (conn_->is_open()) {
            std::cout << "Connected to database successfully." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        throw;
    }
}

void Database::initialize() {
    try {
        pqxx::work txn(*conn_);
        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS entries (
                id SERIAL PRIMARY KEY,
                month DATE NOT NULL,
                type VARCHAR(10) NOT NULL CHECK (type IN ('expense', 'income', 'account_state')),
                name VARCHAR(255) NOT NULL,
                value DECIMAL(10, 2) NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.exec("CREATE INDEX IF NOT EXISTS idx_entries_month ON entries(month)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_entries_type ON entries(type)");
        
        txn.commit();
        std::cout << "Database initialized successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Database initialization failed: " << e.what() << std::endl;
        throw;
    }
}

std::vector<Entry> Database::entry_info(int id) {
    std::vector<Entry> entries;
    
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result res = txn.exec(
            "SELECT id, month, type, name, value, created_at FROM entries WHERE id = $1 ORDER BY created_at DESC",
            // std::vector<std::string>{month}
            pqxx::params(id)
        );
        
        for (const auto& row : res) {
            Entry entry;
            entry.id = row["id"].as<int>();
            entry.month = row["month"].as<std::string>();
            entry.type = row["type"].as<std::string>();
            entry.name = row["name"].as<std::string>();
            entry.value = row["value"].as<double>();
            entry.created_at = row["created_at"].as<std::string>();
            entries.push_back(entry);
        }
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Failed to retrieve entry information: " << e.what() << std::endl;
    }
    
    return entries;
}


bool Database::add_entry(const std::string& month, const std::string& type,
                         const std::string& name, double value) {
    try {
        pqxx::work txn(*conn_);
        
        std::string query = "INSERT INTO entries (month, type, name, value) VALUES ($1, $2, $3, $4)";
        txn.exec(query, {month, type, name, value});
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to add entry: " << e.what() << std::endl;
        return false;
    }
}

bool Database::delete_entry(const int id){
    try {
        pqxx::work txn(*conn_);

        pqxx::result res = txn.exec(
                "DELETE FROM entries WHERE id = $1",
                pqxx::params(id)
                );

        txn.commit();
        return res.affected_rows() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Failed to delete entry: " << e.what() << std::endl;
        return false;
    }

}

bool Database::entry_exists(const int id, std::string& month){
    try {

        pqxx::work txn(*conn_);
        pqxx::result res = txn.exec(
                "SELECT COUNT(*) FROM entries WHERE id = $1 AND month = $2",
                pqxx::params(id, month)
                );

        int count = res[0][0].as<int>();

        return count > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error checking entry existence: " << e.what() << std::endl;
        return false;
    }
}

bool Database::update_type(const int id, const std::string& type){
    pqxx::work txn(*conn_);

    txn.exec(
        "UPDATE entries SET type = $1 WHERE id = $2",
        pqxx::params(type, id)
        );

    txn.commit();
    return true;
}

bool Database::update_name(const int id, const std::string& name){
    pqxx::work txn(*conn_);
    txn.exec(
            "UPDATE entries SET name = $1 WHERE id = $2",
            pqxx::params(name, id)
            );
    txn.commit();
    return true;
}

bool Database::update_value(const int id, const double value){
    pqxx::work txn(*conn_);
    txn.exec(
            "UPDATE entries SET value = $1 WHERE id = $2",
            pqxx::params(value, id)
            );
    txn.commit();
    return true;
}

std::vector<Entry> Database::get_entries_by_month(const std::string& month) {
    std::vector<Entry> entries;
    
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result res = txn.exec(
            "SELECT id, month, type, name, value, created_at FROM entries WHERE month = $1 ORDER BY created_at DESC",
            // std::vector<std::string>{month}
            pqxx::params(month)
        );
        
        for (const auto& row : res) {
            Entry entry;
            entry.id = row["id"].as<int>();
            entry.month = row["month"].as<std::string>();
            entry.type = row["type"].as<std::string>();
            entry.name = row["name"].as<std::string>();
            entry.value = row["value"].as<double>();
            entry.created_at = row["created_at"].as<std::string>();
            entries.push_back(entry);
        }
        
        txn.commit();
    } catch (const std::exception& e) {
        std::cerr << "Failed to retrieve entries: " << e.what() << std::endl;
    }
    
    return entries;
}

double Database::get_total_income(const std::string& month) {
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result res = txn.exec(
            "SELECT COALESCE(SUM(value), 0) as total FROM entries WHERE month = $1 AND type = 'income'",
            pqxx::params(month)
        );
        
        txn.commit();
        
        if (!res.empty()) {
            return res[0]["total"].as<double>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to get total income: " << e.what() << std::endl;
    }
    
    return 0.0;
}

double Database::get_total_expenses(const std::string& month) {
    try {
        pqxx::work txn(*conn_);
        
        pqxx::result res = txn.exec(
            "SELECT COALESCE(SUM(value), 0) as total FROM entries WHERE month = $1 AND type = 'expense'",
            pqxx::params(month)
        );
        
        txn.commit();
        
        if (!res.empty()) {
            return res[0]["total"].as<double>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to get total expenses: " << e.what() << std::endl;
    }
    
    return 0.0;
}

} // namespace finance
