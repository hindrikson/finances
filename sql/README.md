# Create the database

If not installed install postgres:

- Podman postgres container
    To run a postgres image:

    podman pull docker.io/postgres:15

    set the password:
    export DATABASE_PASSWORD=1234

    - set connection variable
    export DB_CONNECTION_STRING="postgresql://postgres:1234@localhost:5432/development"
    - start the postres server
    podman start postgres-dev


    


    podman run -d \
      --name postgres-dev \
      -e POSTGRES_PASSWORD=1234 \
      -e POSTGRES_USER=postgres \
      -e POSTGRES_DB=development \
      -p 5432:5432 \
      -v /home/ruda/all/postres-data:/var/lib/postgresql/data:Z \
      docker.io/postgres:15  

- Get in the postres server:
podman exec -it postgres-dev bash

psql -U postgres

CREATE DATABASE finances;

\c finances

Run the sql code below:

CREATE TABLE IF NOT EXISTS entries (
    id SERIAL PRIMARY KEY,
    month DATE NOT NULL,
    type VARCHAR(10) NOT NULL CHECK (type IN ('expense', 'income', 'account_state')),
    name VARCHAR(255) NOT NULL,
    value DECIMAL(10, 2) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_entries_month ON entries(month);
CREATE INDEX idx_entries_type ON entries(type);



