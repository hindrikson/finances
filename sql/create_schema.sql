CREATE TABLE IF NOT EXISTS entries (
    id SERIAL PRIMARY KEY,
    month DATE NOT NULL,  -- Store as first day of month (e.g., '2024-11-01')
    type VARCHAR(10) NOT NULL CHECK (type IN ('expense', 'income')),
    name VARCHAR(255) NOT NULL,
    value DECIMAL(10, 2) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_entries_month ON entries(month);
CREATE INDEX idx_entries_type ON entries(type);
