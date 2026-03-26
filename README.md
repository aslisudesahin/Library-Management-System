# Library-Management-System
A comprehensive Library Management System developed in C. Features include book inventory tracking, user authentication (Admin/Student/Staff), and borrowing/return management with file handling.
📚 Library Management System (C Language)
This is a console-based Library Management System developed in C, utilizing file handling for persistent data storage.

🚀 Features
User Roles: Supports three types of users: Admin, Student, and Staff.

Book Management: Admins can add, update, and manage the book inventory (ID, Title, Author, Category).

Borrowing System: Users can borrow up to 3 books, check availability, and track return dates.

Persistent Storage: Data is saved in text files (kitaplar.txt, kullanicilar.txt, oduncKitaplar.txt) to ensure information is kept after closing the program.

Search Functionality: Quickly find books by ID or title.

🛠️ Technical Details
Language: C

Key Libraries: stdio.h, stdlib.h, string.h, time.h, stdbool.h.

Data Structures: Uses custom struct types for Books, Users, and Borrowing records.
