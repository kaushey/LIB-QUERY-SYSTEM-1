# library-db-mysql

A relational database design for a multi-branch library management system,
built in T-SQL. Covers schema design, referential integrity, and a stored
procedure for setting up and reporting on the full database.

## Schema

Seven normalized tables model the core library domain:

| Table | Purpose |
|---|---|
| `tbl_publisher` | Publisher name, address, phone |
| `tbl_book` | Book title, linked to publisher |
| `tbl_book_authors` | Author(s) per book |
| `tbl_library_branch` | Branch name and address |
| `tbl_book_copies` | Number of copies of each book held per branch |
| `tbl_borrower` | Library card holder details |
| `tbl_book_loans` | Active loans: book, branch, borrower, checkout/due dates |

Relationships are enforced with foreign keys (`ON UPDATE CASCADE ON DELETE
CASCADE`) so that updates and deletions stay consistent across branches,
books, and loans.

## Features

- Fully normalized schema (publishers → books → copies → branches → loans).
- Foreign-key constraints with cascading updates/deletes for referential
  integrity.
- Wrapped in a stored procedure (`LibraryManagementSystemProcedure`) that
  creates the database, defines all tables, and seeds sample data.
- Sample queries for inspecting branches, borrowers, loans, and copies.

## Tech Stack

T-SQL (SQL Server)

## How to Run

```sql
-- In SQL Server Management Studio:
EXEC dbo.LibraryManagementSystemProcedure
```

## Repo Structure

```
library-db-mysql/
├── Library Management System.sql   # schema, constraints, sample data
└── README.md
```
