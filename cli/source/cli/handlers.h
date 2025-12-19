#include "database/database.h"
#include <string>
#include <optional>

namespace cli_handlers {

    void add_entry(finance::Database& db, const std::string& month, const std::string& type);
    void delete_entry(finance::Database& db, int id);
    void edit_entry(finance::Database& db, int id,
            std::optional<std::string> type = std::nullopt,
            std::optional<std::string> name = std::nullopt,
            std::optional<double> value = std::nullopt
            );
    void handle_edit_entry(finance::Database& db, std::string& month);
}
