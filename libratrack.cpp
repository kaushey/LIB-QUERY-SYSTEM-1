#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>  // Add this header for numeric_limits
#include <sstream>

using namespace std;

// Forward declarations
class Book;
class Library;

// Function declarations
void displayLibrarianBorrowedBooks(Library& library);

// Account class
class Account {
private:
    int accountId;
    vector<int> borrowedBooks;
    vector<int> borrowingHistory;
    double fineAmount;
    bool finePaid;

public:
    Account(int id) : accountId(id), fineAmount(0.0), finePaid(true) {
        borrowedBooks.clear();
        borrowingHistory.clear();
    }
    
    int getAccountId() const { return accountId; }
    const vector<int>& getBorrowedBooks() const { return borrowedBooks; }
    const vector<int>& getBorrowingHistory() const { return borrowingHistory; }
    double getFineAmount() const { return fineAmount; }
    bool isFinePaid() const { return finePaid; }
    
    void addBorrowedBook(int bookId) {
        borrowedBooks.push_back(bookId);
    }
    
    void removeBorrowedBook(int bookId) {
        borrowedBooks.erase(remove(borrowedBooks.begin(), borrowedBooks.end(), bookId), borrowedBooks.end());
    }
    
    void addToHistory(int bookId) {
        borrowingHistory.push_back(bookId);
    }
    
    void addFine(double amount) {
        fineAmount = amount;  // Changed from += to = to prevent accumulation
        finePaid = false;
    }
    
    void payFine() {
        fineAmount = 0.0;
        finePaid = true;
    }
};

// Add these friend functions before the User class
class User;  // Forward declaration

// Friend functions for date calculations
int calculateDaysBetween(time_t startDate, time_t endDate) {
    return (int)(endDate - startDate) / (24 * 60 * 60);
}

double calculateStudentFine(int daysOverdue) {
    return daysOverdue * 10.0;  // 10 rupees per day for students
}

double calculateFacultyFine(int daysOverdue) {
    return 0.0;  // No fine for faculty
}

// Base User class
class User {
protected:
    int userId;
    string name;
    string role;
    string password;  // Added password field
    Account* account;

public:
    User(int id, const string& n, const string& r, const string& p) 
        : userId(id), name(n), role(r), password(p), account(nullptr) {}
    virtual ~User() {
        if (account) delete account;
    }
    
    int getUserId() const { return userId; }
    string getName() const { return name; }
    string getRole() const { return role; }
    string getPassword() const { return password; }  // Added getter for password
    Account* getAccount() const { return account; }
    void setAccount(Account* acc) { account = acc; }
    
    virtual bool canBorrowBook() const = 0;
    virtual int getMaxBooks() const = 0;
    virtual int getMaxDays() const = 0;
    virtual double calculateFine(int daysOverdue) const = 0;

    bool changePassword(const string& currentPassword, const string& newPassword) {
        if (password != currentPassword) {
            cout << "Current password is incorrect.\n";
            return false;
        }
        password = newPassword;
        return true;
    }
};

// Book class
class Book {
private:
    int bookId;
    string title;
    string author;
    string publisher;
    int year;
    string isbn;
    string status;
    int borrowedBy;
    time_t borrowDate;
    time_t dueDate;

public:
    Book(int id, const string& t, const string& a, const string& p, int y, const string& i)
        : bookId(id), title(t), author(a), publisher(p), year(y), isbn(i),
          status("Available"), borrowedBy(-1), borrowDate(0), dueDate(0) {}
    
    int getBookId() const { return bookId; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    string getIsbn() const { return isbn; }
    string getStatus() const { return status; }
    int getBorrowedBy() const { return borrowedBy; }
    time_t getBorrowDate() const { return borrowDate; }
    time_t getDueDate() const { return dueDate; }
    
    void setStatus(const string& s) { status = s; }
    void setBorrowedBy(int id) { borrowedBy = id; }
    void setBorrowDate(time_t date) { borrowDate = date; }
    void setDueDate(time_t date) { dueDate = date; }
};

// Derived User classes
class Student : public User {
private:
    bool hasOverdueBook;  // Track if any book is overdue by 15+ days

public:
    Student(int id, const string& name, const string& password) 
        : User(id, name, "Student", password), hasOverdueBook(false) {}
    
    bool canBorrowBook() const override {
        if (!account) return false;
        
        // Check if reached max books
        if (account->getBorrowedBooks().size() >= getMaxBooks()) {
            cout << "Error: Cannot borrow more books. Maximum limit (3) reached.\n";
            return false;
        }
        
        // Check if has any book overdue by 15+ days
        if (hasOverdueBook) {
            cout << "Error: Cannot borrow books. You have a book overdue for more than 15 days.\n";
            cout << "Please return all overdue books before borrowing new ones.\n";
            return false;
        }
        
        return true;
    }
    
    int getMaxBooks() const override { return 3; }  // Changed from 1 to 3
    int getMaxDays() const override { return 15; }
    
    double calculateFine(int daysOverdue) const override {
        return daysOverdue * 10.0; // 10 rupees per day
    }

    void setHasOverdueBook(bool value) { hasOverdueBook = value; }
    bool getHasOverdueBook() const { return hasOverdueBook; }
};

class Faculty : public User {
private:
    bool hasOverdueBook;  // Track if any book is overdue by 60+ days

public:
    Faculty(int id, const string& name, const string& password) 
        : User(id, name, "Faculty", password), hasOverdueBook(false) {}
    
    bool canBorrowBook() const override {
        if (!account) return false;
        
        // Check if reached max books
        if (account->getBorrowedBooks().size() >= getMaxBooks()) {
            cout << "Error: Cannot borrow more books. Maximum limit (5) reached.\n";
            return false;
        }
        
        // Check if has any book overdue by 60+ days
        if (hasOverdueBook) {
            cout << "Error: Cannot borrow books. You have a book overdue for more than 60 days.\n";
            cout << "Please return all overdue books before borrowing new ones.\n";
            return false;
        }
        
        return true;
    }
    
    int getMaxBooks() const override { return 5; }
    int getMaxDays() const override { return 30; }  // Initial borrowing period
    int getBufferDays() const { return 30; }  // Additional buffer period
    
    double calculateFine(int daysOverdue) const override {
        return 0.0; // No fine for faculty
    }

    void setHasOverdueBook(bool value) { hasOverdueBook = value; }
    bool getHasOverdueBook() const { return hasOverdueBook; }
};

class Librarian : public User {
public:
    Librarian(int id, const string& name, const string& password) 
        : User(id, name, "Librarian", password) {}
    
    bool canBorrowBook() const override { return false; }
    int getMaxBooks() const override { return 0; }
    int getMaxDays() const override { return 0; }
    double calculateFine(int daysOverdue) const override { return 0.0; }
};

// Library class
class Library {
private:
    vector<Book> books;
    vector<User*> users;
    string booksFile;
    string usersFile;
    string accountsFile;

    // New validation methods
    bool isValidISBN(const string& isbn) const {
        // Basic ISBN validation (10 or 13 digits with optional hyphens)
        string cleanISBN = isbn;
        cleanISBN.erase(remove(cleanISBN.begin(), cleanISBN.end(), '-'), cleanISBN.end());
        return cleanISBN.length() == 10 || cleanISBN.length() == 13;
    }

    bool isDuplicateBookId(int bookId) const {
        return findBook(bookId) != nullptr;
    }

    bool isDuplicateUserId(int userId) const {
        return findUser(userId) != nullptr;
    }

    bool isValidDate(int day, int month, int year) const {
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1) return false;
        
        // Check days in month
        int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
            daysInMonth[1] = 29; // Leap year
        }
        return day <= daysInMonth[month - 1];
    }

    bool isDateInPast(int day, int month, int year) const {
        time_t now = time(nullptr);
        struct tm* current = localtime(&now);
        
        if (year < current->tm_year + 1900) return true;
        if (year == current->tm_year + 1900 && month < current->tm_mon + 1) return true;
        if (year == current->tm_year + 1900 && month == current->tm_mon + 1 && day < current->tm_mday) return true;
        return false;
    }

public:
    Library() : booksFile("catalog.txt"), usersFile("members.txt"), accountsFile("borrow_records.txt") {
        // Initialize the library by loading existing data or creating new data
        loadData();
    }
    
    ~Library() {
        saveData();
        for (User* user : users) {
            delete user;
        }
    }
    
    void loadData() {
        // Check if files exist and have content
        ifstream test_books(booksFile);
        ifstream test_users(usersFile);
        ifstream test_accounts(accountsFile);
        
        bool needsInitialization = true;
        
        // Only initialize if ALL files don't exist or are empty
        if (test_books.good() && test_users.good() && test_accounts.good()) {
            string line;
            if (getline(test_books, line) && getline(test_users, line) && getline(test_accounts, line)) {
                needsInitialization = false;
            }
        }
        
        test_books.close();
        test_users.close();
        test_accounts.close();

        // Clear existing data before loading
        books.clear();
        for (User* user : users) {
            delete user;
        }
        users.clear();

        if (needsInitialization) {
            cout << "Initializing library system with default data...\n";
            
            // Add initial books with updated details
            addBook(Book(1, "The Alchemist", "Paulo Coelho", "HarperTorch", 1988, "978-0062315007"));
            addBook(Book(2, "Sapiens", "Yuval Noah Harari", "Harvill Secker", 2011, "978-0062316097"));
            addBook(Book(3, "The Guide", "R. K. Narayan", "Methuen", 1958, "978-0143039648"));
            addBook(Book(4, "Train to Pakistan", "Khushwant Singh", "Chatto & Windus", 1956, "978-0143065883"));
            addBook(Book(5, "The White Tiger", "Aravind Adiga", "Atlantic Books", 2008, "978-1416562603"));
            addBook(Book(6, "Wings of Fire", "A. P. J. Abdul Kalam", "Universities Press", 1999, "978-8173711466"));
            addBook(Book(7, "Brave New World", "Aldous Huxley", "Chatto & Windus", 1932, "978-0060850524"));
            addBook(Book(8, "The Hobbit", "J.R.R. Tolkien", "George Allen & Unwin", 1937, "978-0547928227"));
            addBook(Book(9, "Educated", "Tara Westover", "Random House", 2018, "978-0399590504"));
            addBook(Book(10, "Clean Code", "Robert C. Martin", "Prentice Hall", 2008, "978-0132350884"));

            // Add users with passwords
            // Students (6 students)
            addUser(new Student(1, "Ananya Sharma", "student123"));
            addUser(new Student(2, "Rohan Verma", "student456"));
            addUser(new Student(3, "Priya Nair", "student789"));
            addUser(new Student(4, "Karan Mehta", "student321"));
            addUser(new Student(5, "Sneha Iyer", "student654"));
            addUser(new Student(6, "Aditya Rao", "student987"));

            // Faculty (3 faculty)
            addUser(new Faculty(7, "Dr. Neha Kapoor", "faculty123"));
            addUser(new Faculty(8, "Dr. Sanjay Gupta", "faculty456"));
            addUser(new Faculty(9, "Dr. Meera Joshi", "faculty789"));

            // Librarian (ID: 10)
            addUser(new Librarian(10, "Arjun Malhotra", "librarian123"));
            
            // Save the initial data
            saveData();
        } else {
            // Load existing data from files
            
            // Load books
            ifstream bookFile(booksFile);
            if (bookFile.is_open()) {
                string line;
                while (getline(bookFile, line)) {
                    if(line.empty()) continue;
                    stringstream ss(line);
                    string token;
                    vector<string> tokens;
                    
                    while (getline(ss, token, '|')) {
                        tokens.push_back(token);
                    }
                    
                    if (tokens.size() >= 10) {
                        int id = stoi(tokens[0]);
                        string title = tokens[1];
                        string author = tokens[2];
                        string publisher = tokens[3];
                        int year = stoi(tokens[4]);
                        string isbn = tokens[5];
                        string status = tokens[6];
                        int borrowedBy = stoi(tokens[7]);
                        time_t borrowDate = stoull(tokens[8]);
                        time_t dueDate = stoull(tokens[9]);
                        
                        Book book(id, title, author, publisher, year, isbn);
                        book.setStatus(status);
                        book.setBorrowedBy(borrowedBy);
                        book.setBorrowDate(borrowDate);
                        book.setDueDate(dueDate);
                        books.push_back(book);
                    }
                }
                bookFile.close();
            }

            // Load users
            ifstream userFile(usersFile);
            if (userFile.is_open()) {
                string line;
                while (getline(userFile, line)) {
                    if(line.empty()) continue;
                    stringstream ss(line);
                    string token;
                    vector<string> tokens;
                    
                    while (getline(ss, token, '|')) {
                        tokens.push_back(token);
                    }
                    
                    if (tokens.size() >= 5) {
                        int id = stoi(tokens[0]);
                        string name = tokens[1];
                        string role = tokens[2];
                        string password = tokens[3];
                        bool hasOverdueBook = (tokens[4] == "1");
                        
                        User* user = nullptr;
                        if (role == "Student") {
                            user = new Student(id, name, password);
                            dynamic_cast<Student*>(user)->setHasOverdueBook(hasOverdueBook);
                        } else if (role == "Faculty") {
                            user = new Faculty(id, name, password);
                            dynamic_cast<Faculty*>(user)->setHasOverdueBook(hasOverdueBook);
                        } else if (role == "Librarian") {
                            user = new Librarian(id, name, password);
                        }
                        
                        if (user) {
                            Account* newAccount = new Account(id);
                            user->setAccount(newAccount);
                            users.push_back(user);
                        }
                    }
                }
                userFile.close();
            }

            // Load accounts
            ifstream accountFile(accountsFile);
            if (accountFile.is_open()) {
                string line;
                while (getline(accountFile, line)) {
                    if(line.empty()) continue;
                    stringstream ss(line);
                    string token;
                    vector<string> tokens;
                    
                    while (getline(ss, token, '|')) {
                        tokens.push_back(token);
                    }
                    
                    if (tokens.size() >= 5) {
                        int accountId = stoi(tokens[0]);
                        double fineAmount = stod(tokens[1]);
                        bool finePaid = (tokens[2] == "1");
                        int numBorrowedBooks = stoi(tokens[3]);
                        int numHistoryBooks = stoi(tokens[4]);
                        
                        User* user = findUser(accountId);
                        if (user && user->getAccount()) {
                            Account* account = user->getAccount();
                            
                            for (int i = 0; i < numBorrowedBooks && i + 5 < tokens.size(); i++) {
                                account->addBorrowedBook(stoi(tokens[i + 5]));
                            }
                            
                            int historyStart = 5 + numBorrowedBooks;
                            for (int i = 0; i < numHistoryBooks && historyStart + i < tokens.size(); i++) {
                                account->addToHistory(stoi(tokens[historyStart + i]));
                            }
                            
                            if (fineAmount > 0) {
                                account->addFine(fineAmount);
                            }
                            if (finePaid) {
                                account->payFine();
                            }
                        }
                    }
                }
                accountFile.close();
            }
            
            calculateFines();
        }
    }
    
    void saveData() {
        // Save books with proper date formatting
        ofstream bookFile(booksFile);
        if (bookFile.is_open()) {
            for (const Book& book : books) {
                bookFile << book.getBookId() << "|"
                        << book.getTitle() << "|"
                        << book.getAuthor() << "|"
                        << book.getPublisher() << "|"
                        << book.getYear() << "|"
                        << book.getIsbn() << "|"
                        << book.getStatus() << "|"
                        << book.getBorrowedBy() << "|"
                        << book.getBorrowDate() << "|"
                        << book.getDueDate() << endl;
            }
            bookFile.close();
        }
        
        // Save users with passwords
        ofstream userFile(usersFile);
        if (userFile.is_open()) {
            for (const User* user : users) {
                bool hasOverdueBook = false;
                if (user->getRole() == "Student") {
                    hasOverdueBook = dynamic_cast<const Student*>(user)->getHasOverdueBook();
                } else if (user->getRole() == "Faculty") {
                    hasOverdueBook = dynamic_cast<const Faculty*>(user)->getHasOverdueBook();
                }
                
                userFile << user->getUserId() << "|"
                        << user->getName() << "|"
                        << user->getRole() << "|"
                        << user->getPassword() << "|"  // Save password
                        << hasOverdueBook << endl;
            }
            userFile.close();
        }

        // Save accounts with detailed borrowing information
        ofstream accountFile(accountsFile);
        if (accountFile.is_open()) {
            for (const User* user : users) {
                if (Account* account = user->getAccount()) {
                    accountFile << user->getUserId() << "|"
                              << account->getFineAmount() << "|"
                              << account->isFinePaid() << "|"
                              << account->getBorrowedBooks().size() << "|"
                              << account->getBorrowingHistory().size();
                    
                    // Save borrowed books
                    for (int bookId : account->getBorrowedBooks()) {
                        accountFile << "|" << bookId;
                    }
                    
                    // Save borrowing history
                    for (int bookId : account->getBorrowingHistory()) {
                        accountFile << "|" << bookId;
                    }
                    
                    accountFile << endl;
                }
            }
            accountFile.close();
        }
    }
    
    void addBook(const Book& book) {
        // Validate book details
        if (book.getBookId() <= 0) {
            cout << "Error: Invalid book ID. ID must be positive.\n";
            return;
        }
        
        if (isDuplicateBookId(book.getBookId())) {
            cout << "Error: Book ID already exists.\n";
            return;
        }
        
        if (book.getTitle().empty() || book.getAuthor().empty()) {
            cout << "Error: Title and author cannot be empty.\n";
            return;
        }
        
        if (!isValidISBN(book.getIsbn())) {
            cout << "Error: Invalid ISBN format.\n";
            return;
        }
        
        if (book.getYear() > time(nullptr) / (24 * 60 * 60) / 365 + 1970) {
            cout << "Error: Publication year cannot be in the future.\n";
            return;
        }
        
        // If all validations pass, add the book
        books.push_back(book);
        saveData();
        cout << "Book added successfully!\n";
    }
    
    void removeBook(int bookId) {
        Book* book = findBook(bookId);
        if (!book) {
            cout << "Error: Book not found.\n";
            return;
        }
        
        // Check if book is currently borrowed
        if (book->getStatus() == "Borrowed") {
            User* borrower = findUser(book->getBorrowedBy());
            cout << "Error: Cannot delete book. Book is currently borrowed by ";
            if (borrower) {
                cout << borrower->getName() << " (ID: " << borrower->getUserId() << ").\n";
            } else {
                cout << "a user.\n";
            }
            cout << "Please wait until the book is returned.\n";
            return;
        }
        
        books.erase(remove_if(books.begin(), books.end(),
            [bookId](const Book& b) { return b.getBookId() == bookId; }), books.end());
        saveData();
        cout << "Book removed successfully!\n";
    }
    
    Book* findBook(int bookId) const {
        auto it = find_if(books.begin(), books.end(),
            [bookId](const Book& b) { return b.getBookId() == bookId; });
        return it != books.end() ? const_cast<Book*>(&(*it)) : nullptr;
    }
    
    void addUser(User* user) {
        // Validate user details
        if (user->getUserId() <= 0) {
            cout << "Error: Invalid user ID. ID must be positive.\n";
            delete user;
            return;
        }
        
        if (isDuplicateUserId(user->getUserId())) {
            cout << "Error: User ID already exists.\n";
            delete user;
            return;
        }
        
        if (user->getName().empty()) {
            cout << "Error: User name cannot be empty.\n";
            delete user;
            return;
        }
        
        if (user->getRole() != "Student" && user->getRole() != "Faculty" && user->getRole() != "Librarian") {
            cout << "Error: Invalid user role.\n";
            delete user;
            return;
        }
        
        // If all validations pass, add the user
        Account* newAccount = new Account(user->getUserId());
        user->setAccount(newAccount);
        
        users.push_back(user);
        saveData();
        cout << "User added successfully!\n";
    }
    
    void removeUser(int userId) {
        User* user = findUser(userId);
        if (!user) {
            cout << "Error: User not found.\n";
            return;
        }
        
        // Prevent deleting librarians
        if (user->getRole() == "Librarian") {
            cout << "Error: Cannot delete librarian accounts.\n";
            return;
        }
        
        // Check if user has borrowed books
        Account* account = user->getAccount();
        if (account && !account->getBorrowedBooks().empty()) {
            cout << "Error: Cannot delete user. User has borrowed books.\n";
            cout << "Please ensure all books are returned before deleting the user.\n";
            cout << "Borrowed books:\n";
            for (int bookId : account->getBorrowedBooks()) {
                Book* book = findBook(bookId);
                if (book) {
                    cout << "- " << book->getTitle() << "\n";
                }
            }
            return;
        }
        
        users.erase(remove_if(users.begin(), users.end(),
            [userId](const User* u) { return u->getUserId() == userId; }), users.end());
        saveData();
        cout << "User removed successfully!\n";
    }
    
    User* findUser(int userId) const {
        auto it = find_if(users.begin(), users.end(),
            [userId](const User* u) { return u->getUserId() == userId; });
        return it != users.end() ? *it : nullptr;
    }
    
    bool borrowBook(int userId, int bookId) {
        User* user = findUser(userId);
        Book* book = findBook(bookId);
        
        if (!user || !book) {
            cout << "Error: User or book not found.\n";
            return false;
        }
        
        if (book->getStatus() != "Available") {
            cout << "Error: Book is not available for borrowing.\n";
            return false;
        }
        
        // Get current time as borrow time
        time_t now = time(nullptr);
        struct tm* currentTime = localtime(&now);
        
        // Create a new tm struct for the due date
        struct tm dueDate = *currentTime;
        
        // Add days based on user type
        int daysToAdd = user->getMaxDays();  // 15 for students, 30 for faculty
        
        // Adjust the month and year if necessary when adding days
        dueDate.tm_mday += daysToAdd;
        mktime(&dueDate);  // Normalize the date
        
        if (currentTime) {
            cout << "Borrowing date (Today): " 
                 << setfill('0') << setw(2) << currentTime->tm_mday << "/"
                 << setfill('0') << setw(2) << (currentTime->tm_mon + 1) << "/"
                 << (currentTime->tm_year + 1900) << "\n";
            
            cout << "Book will be due on: " 
                 << setfill('0') << setw(2) << dueDate.tm_mday << "/"
                 << setfill('0') << setw(2) << (dueDate.tm_mon + 1) << "/"
                 << (dueDate.tm_year + 1900) << "\n";
            
            book->setStatus("Borrowed");
            book->setBorrowedBy(userId);
            book->setBorrowDate(now);
            book->setDueDate(mktime(&dueDate));
            
            Account* account = user->getAccount();
            if (account) {
                account->addBorrowedBook(bookId);
                saveData();
                return true;
            } else {
                cout << "Error: Account not found.\n";
                return false;
            }
        } else {
            cout << "Error: Invalid date conversion.\n";
            return false;
        }
    }
    
    bool returnBook(int userId, int bookId) {
        User* user = findUser(userId);
        Book* book = findBook(bookId);
        
        if (!user || !book) {
            cout << "Error: User or book not found.\n";
            return false;
        }
        
        if (book->getBorrowedBy() != userId) {
            cout << "Error: This book is not borrowed by you.\n";
            return false;
        }
        
        // Get current time as return time
        time_t now = time(nullptr);
        struct tm* returnTime = localtime(&now);
        
        // Calculate days overdue
        int daysOverdue = max(0, (int)(now - book->getDueDate()) / (24 * 60 * 60));
        
        // Calculate and apply fine if overdue
        if (daysOverdue > 0) {
            double fine = user->calculateFine(daysOverdue);
            if (fine > 0) {
                Account* account = user->getAccount();
                if (account) {
                    account->addFine(fine);
                    cout << "Fine added: " << fixed << setprecision(2) << fine << " rupees for " 
                         << daysOverdue << " days overdue.\n";
                }
            }
        }
        
        // Update book status
        book->setStatus("Available");
        book->setBorrowedBy(-1);
        book->setBorrowDate(0);
        book->setDueDate(0);
        
        // Update user's account
        Account* account = user->getAccount();
        if (account) {
            account->removeBorrowedBook(bookId);
            account->addToHistory(bookId);
            
            // Save all changes to files
            saveData();
            
            cout << "Book returned on: " 
                 << setfill('0') << setw(2) << returnTime->tm_mday << "/"
                 << setfill('0') << setw(2) << (returnTime->tm_mon + 1) << "/"
                 << (returnTime->tm_year + 1900) << "\n";
            
            return true;
        } else {
            cout << "Error: Account not found.\n";
            return false;
        }
    }
    
    void calculateFines() {
        time_t now = time(nullptr);
        for (User* user : users) {
            Account* account = user->getAccount();
            if (!account) continue;
            
            for (int bookId : account->getBorrowedBooks()) {
                Book* book = findBook(bookId);
                if (!book) continue;
                
                int daysOverdue = max(0, (int)(now - book->getDueDate()) / (24 * 60 * 60));
                if (daysOverdue > 0) {
                    double fine = user->calculateFine(daysOverdue);
                    account->addFine(fine);
                }
            }
        }
    }
    
    vector<pair<Book*, User*>> getBorrowedBooks() const {
        vector<pair<Book*, User*>> borrowedBooks;
        for (const Book& book : books) {
            if (book.getStatus() == "Borrowed") {
                User* borrower = findUser(book.getBorrowedBy());
                if (borrower) {
                    borrowedBooks.push_back({const_cast<Book*>(&book), borrower});
                }
            }
        }
        return borrowedBooks;
    }
    
    const vector<Book>& getBooks() const { return books; }
    const vector<User*>& getUsers() const { return users; }
};

// Helper functions
void displayStudentMenu(Library& library, User* user) {
    if (!user) {
        cout << "Error: Invalid user pointer.\n";
        return;
    }

    int choice;
    do {
        cout << "\nStudent Menu\n";
        cout << "1. Borrow a book\n";
        cout << "2. Return a book\n";
        cout << "3. View all books\n";
        cout << "4. View my details\n";
        cout << "5. Pay fine\n";
        cout << "6. View all currently borrowed books in library\n";
        cout << "7. View your borrowing history\n";
        cout << "8. Change password\n";
        cout << "9. Logout\n";
        cout << "Enter your choice: ";
        
        // Clear any previous input errors
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (!(cin >> choice)) {
            cout << "Error: Please enter a valid number.\n";
            continue;
        }
        
        switch (choice) {
            case 1: {
                if (!user->canBorrowBook()) {
                    break;
                }
                
                int bookId;
                string input;
                bool validInput = false;
                
                do {
                    cout << "Enter book ID to borrow: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    getline(cin, input);
                    
                    // Check if input contains only digits
                    bool isNumeric = true;
                    for (char c : input) {
                        if (!isdigit(c)) {
                            isNumeric = false;
                            break;
                        }
                    }
                    
                    if (!isNumeric || input.empty()) {
                        cout << "Error: Book ID must be a positive integer (no dates, decimals, or special characters).\n";
                        continue;
                    }
                    
                    try {
                        bookId = stoi(input);
                        if (bookId <= 0) {
                            cout << "Error: Book ID must be a positive integer.\n";
                            continue;
                        }
                        validInput = true;
                    } catch (const exception&) {
                        cout << "Error: Invalid book ID format.\n";
                        continue;
                    }
                } while (!validInput);
                
                if (library.borrowBook(user->getUserId(), bookId)) {
                    cout << "Book borrowed successfully!\n";
                } else {
                    cout << "Failed to borrow book.\n";
                }
                break;
            }
            case 2: {
                int bookId;
                cout << "Enter book ID to return: ";
                
                // Clear any previous input errors
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (!(cin >> bookId) || bookId <= 0) {
                    cout << "Error: Book ID must be a positive integer.\n";
                    break;
                }
                
                if (library.returnBook(user->getUserId(), bookId)) {
                    cout << "Book returned successfully!\n";
                } else {
                    cout << "Failed to return book.\n";
                }
                break;
            }
            case 3: {
                cout << "\nAll Books:\n";
                const vector<Book>& books = library.getBooks();
                for (const Book& book : books) {
                    cout << "ID: " << book.getBookId() << ", Title: " << book.getTitle()
                         << ", Author: " << book.getAuthor() << ", Status: " << book.getStatus() << "\n";
                }
                break;
            }
            case 4: {
                cout << "\nMy Details:\n";
                cout << "ID: " << user->getUserId() << "\n";
                cout << "Name: " << user->getName() << "\n";
                cout << "Role: " << user->getRole() << "\n";
                
                Account* account = user->getAccount();
                if (!account) {
                    cout << "Error: Account not found.\n";
                    break;
                }

                cout << "Fine Amount: " << fixed << setprecision(2) << account->getFineAmount() << " rupees\n";
                cout << "Fine Status: " << (account->isFinePaid() ? "Paid" : "Unpaid") << "\n";
                
                cout << "\nCurrently Borrowed Books:\n";
                const vector<int>& borrowed = account->getBorrowedBooks();
                if (borrowed.empty()) {
                    cout << "No books currently borrowed.\n";
                } else {
                    bool hasValidBooks = false;
                    for (int bookId : borrowed) {
                        Book* book = library.findBook(bookId);
                        if (!book || book->getStatus() != "Borrowed" || book->getBorrowedBy() != user->getUserId()) {
                            continue;
                        }
                        hasValidBooks = true;

                        cout << "\nBook ID: " << book->getBookId() << "\n";
                        cout << "Title: " << book->getTitle() << "\n";
                        cout << "Author: " << book->getAuthor() << "\n";
                        
                        time_t borrowTime = book->getBorrowDate();
                        time_t dueTime = book->getDueDate();
                        
                        if (borrowTime != 0 && dueTime != 0) {
                            struct tm* borrowDate = localtime(&borrowTime);
                            struct tm* dueDate = localtime(&dueTime);
                            
                            if (borrowDate && dueDate) {
                                cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                                     << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                                     << (borrowDate->tm_year + 1900) << "\n";
                                
                                cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                                     << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                                     << (dueDate->tm_year + 1900) << "\n";
                                
                                // Calculate days overdue
                                time_t now = time(nullptr);
                                int daysOverdue = max(0, (int)(now - dueTime) / (24 * 60 * 60));
                                if (daysOverdue > 0) {
                                    cout << "Days overdue: " << daysOverdue << "\n";
                                    cout << "Fine amount: " << fixed << setprecision(2) << user->calculateFine(daysOverdue) << " rupees\n";
                                }
                            }
                        }
                        cout << "------------------------\n";
                    }
                    if (!hasValidBooks) {
                        cout << "No valid borrowed books found.\n";
                    }
                }
                break;
            }
            case 5: {
                Account* account = user->getAccount();
                if (!account) {
                    cout << "Error: Account not found.\n";
                    break;
                }
                
                if (!account->isFinePaid()) {
                    cout << "Current fine amount: " << fixed << setprecision(2) << account->getFineAmount() << " rupees\n";
                    account->payFine();
                    cout << "Fine paid successfully!\n";
                } else {
                    cout << "No unpaid fines found.\n";
                }
                break;
            }
            case 6: {
                cout << "\nAll Currently Borrowed Books in Library:\n";
                const vector<Book>& allBooks = library.getBooks();
                bool foundBorrowed = false;
                
                for (const Book& book : allBooks) {
                    if (book.getStatus() == "Borrowed") {
                        foundBorrowed = true;
                        cout << "\nBook ID: " << book.getBookId() << "\n";
                        cout << "Title: " << book.getTitle() << "\n";
                        cout << "Author: " << book.getAuthor() << "\n";
                        cout << "Status: Borrowed\n";
                        
                        time_t borrowTime = book.getBorrowDate();
                        time_t dueTime = book.getDueDate();
                        if (borrowTime != 0 && dueTime != 0) {
                            struct tm* borrowDate = localtime(&borrowTime);
                            struct tm* dueDate = localtime(&dueTime);
                            
                            cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                                 << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                                 << (borrowDate->tm_year + 1900) << "\n";
                            
                            cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                                 << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                                 << (dueDate->tm_year + 1900) << "\n";
                        }
                        cout << "------------------------\n";
                    }
                }
                
                if (!foundBorrowed) {
                    cout << "No books are currently borrowed.\n";
                }
                break;
            }
            case 7: {
                cout << "\nYour Borrowing History:\n";
                Account* account = user->getAccount();
                if (!account) {
                    cout << "Error: Account not found.\n";
                    break;
                }

                // Get both current borrowed books and history
                const vector<int>& currentlyBorrowed = account->getBorrowedBooks();
                const vector<int>& history = account->getBorrowingHistory();

                if (currentlyBorrowed.empty() && history.empty()) {
                    cout << "No borrowing history found.\n";
                    break;
                }

                // Show currently borrowed books
                if (!currentlyBorrowed.empty()) {
                    cout << "\nCurrently Borrowed Books:\n";
                    for (int bookId : currentlyBorrowed) {
                        Book* book = library.findBook(bookId);
                        if (!book) continue;

                        cout << "\nBook ID: " << book->getBookId() << "\n";
                        cout << "Title: " << book->getTitle() << "\n";
                        cout << "Author: " << book->getAuthor() << "\n";
                        
                        time_t borrowTime = book->getBorrowDate();
                        time_t dueTime = book->getDueDate();
                        if (borrowTime != 0 && dueTime != 0) {
                            struct tm* borrowDate = localtime(&borrowTime);
                            struct tm* dueDate = localtime(&dueTime);
                            
                            cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                                 << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                                 << (borrowDate->tm_year + 1900) << "\n";
                            
                            cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                                 << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                                 << (dueDate->tm_year + 1900) << "\n";
                            
                            // Show if book is overdue
                            time_t now = time(nullptr);
                            int daysOverdue = max(0, (int)(now - dueTime) / (24 * 60 * 60));
                            if (daysOverdue > 0) {
                                cout << "Status: Overdue by " << daysOverdue << " days\n";
                            } else {
                                cout << "Status: Currently borrowed\n";
                            }
                        }
                        cout << "------------------------\n";
                    }
                }

                // Show previously returned books
                if (!history.empty()) {
                    cout << "\nPreviously Returned Books:\n";
                    for (int bookId : history) {
                        // Skip if book is currently borrowed (to avoid duplication)
                        if (find(currentlyBorrowed.begin(), currentlyBorrowed.end(), bookId) != currentlyBorrowed.end()) {
                            continue;
                        }

                        Book* book = library.findBook(bookId);
                        if (!book) continue;

                        cout << "\nBook ID: " << book->getBookId() << "\n";
                        cout << "Title: " << book->getTitle() << "\n";
                        cout << "Author: " << book->getAuthor() << "\n";
                        cout << "Status: Previously borrowed and returned\n";
                        cout << "------------------------\n";
                    }
                }
                break;
            }
            case 8: {
                string currentPassword, newPassword;
                cout << "Enter current password: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, currentPassword);
                cout << "Enter new password: ";
                getline(cin, newPassword);
                
                if (user->changePassword(currentPassword, newPassword)) {
                    library.saveData();  // Save immediately after password change
                    cout << "Password changed successfully!\n";
                }
                break;
            }
            case 9:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
}

void displayFacultyMenu(Library& library, User* user) {
    int choice;
    do {
        cout << "\nFaculty Menu\n";
        cout << "1. Borrow a book\n";
        cout << "2. Return a book\n";
        cout << "3. View all books\n";
        cout << "4. View my details\n";
        cout << "5. Pay fine\n";
        cout << "6. View all currently borrowed books in library\n";
        cout << "7. View your borrowing history\n";
        cout << "8. Change password\n";
        cout << "9. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1: {
                Account* account = user->getAccount();
                if (account && !account->isFinePaid()) {
                    cout << "Error: You have unpaid fines of " << account->getFineAmount() << " rupees.\n";
                    cout << "Please pay your fines first before borrowing books.\n";
                    break;
                }
                
                int bookId;
                cout << "Enter book ID to borrow: ";
                
                // Clear any previous input errors
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (!(cin >> bookId) || bookId <= 0) {
                    cout << "Error: Book ID must be a positive integer.\n";
                    break;
                }
                
                if (library.borrowBook(user->getUserId(), bookId)) {
                    cout << "Book borrowed successfully!\n";
                } else {
                    cout << "Failed to borrow book.\n";
                }
                break;
            }
            case 2: {
                int bookId;
                cout << "Enter book ID to return: ";
                
                // Clear any previous input errors
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (!(cin >> bookId) || bookId <= 0) {
                    cout << "Error: Book ID must be a positive integer.\n";
                    break;
                }
                
                if (library.returnBook(user->getUserId(), bookId)) {
                    cout << "Book returned successfully!\n";
                } else {
                    cout << "Failed to return book.\n";
                }
                break;
            }
            case 3: {
                cout << "\nAll Books:\n";
                for (const Book& book : library.getBooks()) {
                    cout << "ID: " << book.getBookId() << ", Title: " << book.getTitle()
                         << ", Author: " << book.getAuthor() << ", Status: " << book.getStatus() << "\n";
                }
                break;
            }
            case 4: {
                cout << "\nMy Details:\n";
                cout << "ID: " << user->getUserId() << "\n";
                cout << "Name: " << user->getName() << "\n";
                cout << "Role: " << user->getRole() << "\n";
                
                Account* account = user->getAccount();
                if (account) {
                    cout << "Fine Amount: " << account->getFineAmount() << "\n";
                    cout << "Fine Status: " << (account->isFinePaid() ? "Paid" : "Unpaid") << "\n";
                    
                    cout << "Borrowed Books:\n";
                    for (int bookId : account->getBorrowedBooks()) {
                        Book* book = library.findBook(bookId);
                        if (book) {
                            cout << "- " << book->getTitle() << "\n";
                        }
                    }
                }
                break;
            }
            case 5: {
                Account* account = user->getAccount();
                if (account && !account->isFinePaid()) {
                    cout << "Current fine amount: " << account->getFineAmount() << "\n";
                    account->payFine();
                    cout << "Fine paid successfully!\n";
                } else {
                    cout << "No unpaid fines found.\n";
                }
                break;
            }
            case 6: {
                cout << "\nAll Currently Borrowed Books in Library:\n";
                const vector<Book>& allBooks = library.getBooks();
                bool foundBorrowed = false;
                
                for (const Book& book : allBooks) {
                    if (book.getStatus() == "Borrowed") {
                        foundBorrowed = true;
                        cout << "\nBook ID: " << book.getBookId() << "\n";
                        cout << "Title: " << book.getTitle() << "\n";
                        cout << "Author: " << book.getAuthor() << "\n";
                        cout << "Status: Borrowed\n";
                        
                        time_t borrowTime = book.getBorrowDate();
                        time_t dueTime = book.getDueDate();
                        if (borrowTime != 0 && dueTime != 0) {
                            struct tm* borrowDate = localtime(&borrowTime);
                            struct tm* dueDate = localtime(&dueTime);
                            
                            cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                                 << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                                 << (borrowDate->tm_year + 1900) << "\n";
                            
                            cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                                 << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                                 << (dueDate->tm_year + 1900) << "\n";
                        }
                        cout << "------------------------\n";
                    }
                }
                
                if (!foundBorrowed) {
                    cout << "No books are currently borrowed.\n";
                }
                break;
            }
            case 7: {
                cout << "\nBorrowing History:\n";
                Account* account = user->getAccount();
                if (account) {
                    const vector<int>& history = account->getBorrowingHistory();
                    const vector<int>& borrowed = account->getBorrowedBooks();
                    
                    if (history.empty() && borrowed.empty()) {
                        cout << "No borrowing history found.\n";
                        break;
                    }
                    
                    // Display currently borrowed books
                    if (!borrowed.empty()) {
                        cout << "Currently Borrowed Books:\n";
                        for (int bookId : borrowed) {
                            Book* book = library.findBook(bookId);
                            if (!book) {
                                cout << "Error: Book with ID " << bookId << " not found.\n";
                                continue;
                            }

                            cout << "\nBook ID: " << book->getBookId() << "\n";
                            cout << "Title: " << book->getTitle() << "\n";
                            cout << "Author: " << book->getAuthor() << "\n";
                            
                            time_t borrowTime = book->getBorrowDate();
                            time_t dueTime = book->getDueDate();
                            
                            if (borrowTime != 0 && dueTime != 0) {
                                struct tm* borrowDate = localtime(&borrowTime);
                                struct tm* dueDate = localtime(&dueTime);
                                
                                if (borrowDate && dueDate) {
                                    cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                                         << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                                         << (borrowDate->tm_year + 1900) << "\n";
                                    
                                    cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                                         << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                                         << (dueDate->tm_year + 1900) << "\n";
                                    
                                    // Calculate days overdue
                                    time_t now = time(nullptr);
                                    int daysOverdue = max(0, (int)(now - dueTime) / (24 * 60 * 60));
                                    if (daysOverdue > 0) {
                                        cout << "Days overdue: " << daysOverdue << "\n";
                                        cout << "Fine amount: " << fixed << setprecision(2) << user->calculateFine(daysOverdue) << " rupees\n";
                                    }
                                }
                            }
                            cout << "------------------------\n";
                        }
                    }
                    
                    // Display history of returned books
                    if (!history.empty()) {
                        cout << "Returned Books History:\n";
                        for (int bookId : history) {
                            Book* book = library.findBook(bookId);
                            if (!book) {
                                cout << "Error: Book with ID " << bookId << " not found.\n";
                                continue;
                            }

                            cout << "\nBook ID: " << book->getBookId() << "\n";
                            cout << "Title: " << book->getTitle() << "\n";
                            cout << "Author: " << book->getAuthor() << "\n";
                            
                            time_t borrowTime = book->getBorrowDate();
                            time_t dueTime = book->getDueDate();
                            
                            if (borrowTime != 0 && dueTime != 0) {
                                struct tm* borrowDate = localtime(&borrowTime);
                                struct tm* dueDate = localtime(&dueTime);
                                
                                if (borrowDate && dueDate) {
                                    cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                                         << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                                         << (borrowDate->tm_year + 1900) << "\n";
                                    
                                    cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                                         << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                                         << (dueDate->tm_year + 1900) << "\n";
                                    
                                    cout << "Status: Returned\n";
                                }
                            }
                            cout << "------------------------\n";
                        }
                    }
                }
                break;
            }
            case 8: {
                string currentPassword, newPassword;
                cout << "Enter current password: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, currentPassword);
                cout << "Enter new password: ";
                getline(cin, newPassword);
                
                if (user->changePassword(currentPassword, newPassword)) {
                    library.saveData();  // Save immediately after password change
                    cout << "Password changed successfully!\n";
                }
                break;
            }
            case 9:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);
}

void displayLibrarianMenu(Library& library, User* user) {
    int choice;
    do {
        cout << "\nLibrarian Menu\n";
        cout << "1. Add a new book\n";
        cout << "2. Remove a book\n";
        cout << "3. View all books\n";
        cout << "4. View all users\n";
        cout << "5. Add a new user\n";
        cout << "6. Remove a user\n";
        cout << "7. View all borrowed books\n";
        cout << "8. View detailed borrowing information\n";
        cout << "9. Change password\n";
        cout << "10. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "\nAdd Book Options:\n";
                cout << "1. Add a new different book\n";
                cout << "2. Add a copy of existing book\n";
                cout << "Enter your choice (1-2): ";
                
                int addBookChoice;
                cin >> addBookChoice;
                
                if (addBookChoice == 1) {
                    // Add a new different book
                    int id, year;
                    string title, author, publisher, isbn;
                    cout << "Enter book details:\n";
                    cout << "ID: "; 
                    
                    // Clear any previous input errors
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    if (!(cin >> id) || id <= 0) {
                        cout << "Error: Book ID must be a positive integer.\n";
                        break;
                    }
                    
                    cout << "Title: "; cin.ignore(); getline(cin, title);
                    cout << "Author: "; getline(cin, author);
                    cout << "Publisher: "; getline(cin, publisher);
                    cout << "Year: "; cin >> year;
                    cout << "ISBN: "; cin.ignore(); getline(cin, isbn);
                    
                    // Create the book and try to add it
                    Book newBook(id, title, author, publisher, year, isbn);
                    library.addBook(newBook);
                } else if (addBookChoice == 2) {
                    // Add a copy of existing book
                    int existingBookId;
                    cout << "Enter the ID of the book you want to copy: ";
                    cin >> existingBookId;
                    
                    Book* existingBook = library.findBook(existingBookId);
                    if (!existingBook) {
                        cout << "Error: Book with ID " << existingBookId << " not found.\n";
                        break;
                    }
                    
                    int newId;
                    cout << "Enter new ID for the copy: ";
                    cin >> newId;
                    
                    // Create a new book with same details but different ID
                    Book copyBook(newId, 
                                existingBook->getTitle(),
                                existingBook->getAuthor(),
                                existingBook->getPublisher(),
                                existingBook->getYear(),
                                existingBook->getIsbn());
                    library.addBook(copyBook);
                } else {
                    cout << "Invalid choice.\n";
                }
                break;
            }
            case 2: {
                int bookId;
                cout << "Enter book ID to remove: ";
                
                // Clear any previous input errors
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (!(cin >> bookId) || bookId <= 0) {
                    cout << "Error: Book ID must be a positive integer.\n";
                    break;
                }
                
                library.removeBook(bookId);
                break;
            }
            case 3: {
                cout << "\nAll Books:\n";
                for (const Book& book : library.getBooks()) {
                    cout << "ID: " << book.getBookId() << ", Title: " << book.getTitle()
                         << ", Author: " << book.getAuthor() << ", Status: " << book.getStatus() << "\n";
                }
                break;
            }
            case 4: {
                cout << "\nAll Users:\n";
                for (const User* user : library.getUsers()) {
                    cout << "ID: " << user->getUserId() << ", Name: " << user->getName()
                         << ", Role: " << user->getRole() << "\n";
                }
                break;
            }
            case 5: {
                int id;
                string name, role, password;
                cout << "Enter user details:\n";
                cout << "ID: "; cin >> id;
                cout << "Name: "; cin.ignore(); getline(cin, name);
                cout << "Role (Student/Faculty): "; getline(cin, role);
                cout << "Password: "; getline(cin, password);
                
                User* newUser = nullptr;
                if (role == "Student") {
                    newUser = new Student(id, name, password);
                } else if (role == "Faculty") {
                    newUser = new Faculty(id, name, password);
                } else {
                    cout << "Invalid role!\n";
                    break;
                }
                
                // Try to add the user
                library.addUser(newUser);
                break;
            }
            case 6: {
                int userId;
                cout << "Enter user ID to remove: ";
                cin >> userId;
                
                library.removeUser(userId);
                break;
            }
            case 7: {
                displayLibrarianBorrowedBooks(library);
                break;
            }
            case 8: {
                cout << "\nDetailed Borrowing Information:\n";
                auto borrowedBooks = library.getBorrowedBooks();
                
                if (borrowedBooks.empty()) {
                    cout << "No books are currently borrowed.\n";
                    break;
                }
                
                for (const auto& pair : borrowedBooks) {
                    Book* book = pair.first;
                    User* borrower = pair.second;
                    
                    cout << "\nBook Details:\n";
                    cout << "ID: " << book->getBookId() << "\n";
                    cout << "Title: " << book->getTitle() << "\n";
                    cout << "Author: " << book->getAuthor() << "\n";
                    cout << "ISBN: " << book->getIsbn() << "\n";
                    
                    cout << "\nBorrower Details:\n";
                    cout << "ID: " << borrower->getUserId() << "\n";
                    cout << "Name: " << borrower->getName() << "\n";
                    cout << "Role: " << borrower->getRole() << "\n";
                    
                    time_t borrowTime = book->getBorrowDate();
                    time_t dueTime = book->getDueDate();
                    cout << "\nBorrowing Details:\n";
                    cout << "Borrowed on: " << ctime(&borrowTime);
                    cout << "Due on: " << ctime(&dueTime);
                    
                    // Calculate days overdue and fine
                    time_t now = time(nullptr);
                    int daysOverdue = max(0, (int)(now - book->getDueDate()) / (24 * 60 * 60));
                    if (daysOverdue > 0) {
                        cout << "Days overdue: " << daysOverdue << "\n";
                        cout << "Fine amount: " << fixed << setprecision(2) << borrower->calculateFine(daysOverdue) << " rupees\n";
                    }
                    
                    // Show account status
                    Account* account = borrower->getAccount();
                    if (account) {
                        cout << "Account Status:\n";
                        cout << "Total fine amount: " << account->getFineAmount() << " rupees\n";
                        cout << "Fine status: " << (account->isFinePaid() ? "Paid" : "Unpaid") << "\n";
                    }
                    cout << "------------------------\n";
                }
                break;
            }
            case 9: {
                string currentPassword, newPassword;
                cout << "Enter current password: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, currentPassword);
                cout << "Enter new password: ";
                getline(cin, newPassword);
                
                if (user->changePassword(currentPassword, newPassword)) {
                    library.saveData();  // Save immediately after password change
                    cout << "Password changed successfully!\n";
                }
                break;
            }
            case 10:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 10);
}

void displayLibrarianBorrowedBooks(Library& library) {
    cout << "\nAll Currently Borrowed Books in Library:\n";
    auto borrowedBooks = library.getBorrowedBooks();
    if (borrowedBooks.empty()) {
        cout << "No books are currently borrowed.\n";
    } else {
        for (const auto& pair : borrowedBooks) {
            Book* book = pair.first;
            User* borrower = pair.second;
            cout << "\nBook ID: " << book->getBookId() << "\n";
            cout << "Title: " << book->getTitle() << "\n";
            cout << "Author: " << book->getAuthor() << "\n";
            cout << "Borrowed by: " << borrower->getName() << " (ID: " << borrower->getUserId() << ")\n";
            cout << "Status: " << book->getStatus() << "\n";
            
            time_t borrowTime = book->getBorrowDate();
            time_t dueTime = book->getDueDate();
            if (borrowTime != 0 && dueTime != 0) {
                struct tm* borrowDate = localtime(&borrowTime);
                struct tm* dueDate = localtime(&dueTime);
                
                cout << "Borrowed on: " << setfill('0') << setw(2) << borrowDate->tm_mday << "/"
                     << setfill('0') << setw(2) << (borrowDate->tm_mon + 1) << "/"
                     << (borrowDate->tm_year + 1900) << "\n";
                
                cout << "Due on: " << setfill('0') << setw(2) << dueDate->tm_mday << "/"
                     << setfill('0') << setw(2) << (dueDate->tm_mon + 1) << "/"
                     << (dueDate->tm_year + 1900) << "\n";
                
                // Show if book is overdue
                time_t now = time(nullptr);
                int daysOverdue = max(0, (int)(now - dueTime) / (24 * 60 * 60));
                if (daysOverdue > 0) {
                    cout << "Overdue by: " << daysOverdue << " days\n";
                }
            }
            cout << "------------------------\n";
        }
    }
}

int main() {
    Library library;
    library.loadData();  // This will handle initialization if needed
    
    // Main login loop
    while (true) {
        cout << "\nLibraTrack - Library Management System\n";
        cout << "1. Login\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        
        string choiceStr;
        // Clear any leftover newlines in the input buffer
        cin.clear();
        if (cin.peek() == '\n') {
            cin.ignore();
        }
        
        // Get the choice
        getline(cin, choiceStr);
        
        // Remove any whitespace
        choiceStr.erase(remove_if(choiceStr.begin(), choiceStr.end(), ::isspace), choiceStr.end());
        
        // Skip empty input
        if (choiceStr.empty()) {
            continue;
        }
        
        int choice;
        try {
            choice = stoi(choiceStr);
        } catch (...) {
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        if (choice == 2) {
            cout << "Saving all data...\n";
            library.saveData();
            cout << "Thank you for using LibraTrack!\n";
            break;
        }
        
        if (choice == 1) {
            cout << "\nSelect your role:\n";
            cout << "1. Student\n";
            cout << "2. Faculty\n";
            cout << "3. Librarian\n";
            cout << "Enter your role (1-3): ";
            
            string roleInput;
            getline(cin, roleInput);
            int roleChoice;
            try {
                roleChoice = stoi(roleInput);
            } catch (...) {
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            
            string role;
            switch (roleChoice) {
                case 1: role = "Student"; break;
                case 2: role = "Faculty"; break;
                case 3: role = "Librarian"; break;
                default:
                    cout << "Invalid role selection!\n";
                    continue;
            }
            
            cout << "\nEnter your user ID: ";
            string userIdInput;
            getline(cin, userIdInput);
            int userId;
            try {
                userId = stoi(userIdInput);
            } catch (...) {
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            
            cout << "Enter your password: ";
            string password;
            getline(cin, password);
            
            User* user = library.findUser(userId);
            
            // Remove debug output and improve error messages
            if (user && user->getRole() == role && user->getPassword() == password) {
                cout << "Welcome, " << user->getName() << "!\n";
                
                if (role == "Student") {
                    displayStudentMenu(library, user);
                } else if (role == "Faculty") {
                    displayFacultyMenu(library, user);
                } else if (role == "Librarian") {
                    displayLibrarianMenu(library, user);
                }
            } else {
                cout << "Invalid credentials. Please try again.\n";  // Generic error message for security
            }
        }
    }
    
    return 0;
} 
