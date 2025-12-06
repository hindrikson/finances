#pragma once
#include "database/database.h"
#include <string>

namespace cli {
    void display_menu();

    void display_edit_entry_menu();

    void clear_screen();

    void view_summary(finance::Database& db, const std::string& month);

    void view_entries(finance::Database& db, const std::string& month);
}
