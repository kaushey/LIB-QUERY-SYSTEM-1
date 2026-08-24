# LibraTrack 📚

A console-based Library Management System written in C++ that supports role-based access for **Students**, **Faculty**, and **Librarians** — with book borrowing/returns, fine calculation, and persistent file-based storage.

## Features

### 🎓 Student
- Borrow up to 3 books at a time (15-day borrowing period)
- Return books
- Pay fines (₹10/day for overdue books)
- View all books, personal details, currently borrowed books, and borrowing history
- Change password

### 🧑‍🏫 Faculty
- Borrow up to 5 books (30-day period + 30-day grace buffer, no fines)
- Return books
- View all books, personal details, currently borrowed books, and borrowing history
- Change password

### 🗂️ Librarian
- Add / remove books and add extra copies
- Add / remove users
- View all books, all users, and all currently borrowed books
- View detailed borrowing information across the system
- Change password

## Tech Highlights

- Object-oriented design using inheritance (`User` → `Student` / `Faculty` / `Librarian`) and polymorphism for role-specific borrowing rules and fine logic
- Custom file-based persistence layer (pipe-delimited `.txt` storage) for the book catalog, member accounts, and borrowing records — no external database required
- Automatic overdue detection and fine calculation using system date/time
- Input validation for IDs, ISBNs, and dates

## Getting Started

### Prerequisites
- A C++17-compatible compiler (e.g. `g++`)

### Build
```bash
g++ -std=c++17 -o libratrack libratrack.cpp
```

### Run
```bash
./libratrack
```

On first run, LibraTrack seeds the system with a sample catalog of 10 books and 10 members (6 students, 3 faculty, 1 librarian) and writes them to `catalog.txt`, `members.txt`, and `borrow_records.txt`. All further changes are saved automatically.

## Sample Login Credentials

| Role      | Name              | ID | Password       |
|-----------|-------------------|----|-----------------|
| Student   | Ananya Sharma     | 1  | student123      |
| Student   | Rohan Verma       | 2  | student456      |
| Faculty   | Dr. Neha Kapoor   | 7  | faculty123      |
| Librarian | Arjun Malhotra    | 10 | librarian123    |

*(See `members.txt` for the full list after first run.)*

## Borrowing Rules

**Students:** max 3 books, 15-day period, ₹10/day fine if overdue, blocked from new borrows if any book is 15+ days overdue.

**Faculty:** max 5 books, 30-day period + 30-day buffer (60 days total), no fines, must clear books overdue 60+ days before borrowing again.

**Librarian:** cannot borrow — manages the system instead.

## Project Structure
```
LibraTrack/
├── libratrack.cpp       # Main application source
├── catalog.txt          # Book catalog (generated on first run)
├── members.txt          # User accounts (generated on first run)
├── borrow_records.txt   # Borrowing/account records (generated on first run)
├── LICENSE
└── README.md
```

## Possible Improvements
- Migrate flat-file storage to SQLite
- Add a GUI (Qt) or web front end
- Add unit tests for fine calculation and borrowing rules


