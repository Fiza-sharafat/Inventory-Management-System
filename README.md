# Inventory Management System

A console-based **Inventory Management System developed in C++** using Object-Oriented Programming (OOP), file handling, and dynamic data structures.

The system provides separate interfaces for **Admin** and **Customer** users and supports inventory management, product searching, shopping cart operations, checkout, order history, stock monitoring, and inventory reports.

## Features

**Admin Features**
- View product catalog
- Add new products
- Restock products
- Search products by name
- Filter products by type
- Monitor low-stock products
- View inventory statistics and reports
- Manage physical and digital products

### Customer Features
- Browse product catalog
- Search products
- Add products to cart
- Remove products from cart
- View shopping cart
- Calculate total payable amount
- Complete checkout
- View order history

## Technical Concepts

This project demonstrates several C++ programming concepts:

- Object-Oriented Programming (OOP)
- Classes and Objects
- Inheritance
- Polymorphism
- Encapsulation
- Virtual Functions
- Structures
- Vectors
- Pointers
- Dynamic Memory Management
- File Handling
- Input Validation
- String Processing
- Searching and Filtering

## Product Types

The system supports two types of products:

### Physical Products
- 10% tax
- Fixed shipping cost

### Digital Products
- 5% tax
- No shipping cost

The project uses polymorphism to calculate product-specific tax and shipping costs.

## Data Persistence

The system stores inventory and order information using text files:

```text
inventory.txt
orders.txt
```

This allows inventory and order data to be loaded when the program starts and saved when changes are made.

## How to Run

### Option 1 — Run the Executable

Download:

```text
Inventory Management system.exe
```

and run it on a Windows computer.

### Option 2 — Compile the Source Code

You can compile the C++ source code using a C++ compiler such as MinGW/G++.

Example:

```bash
g++ "Inventory Management system.cpp" -o InventoryManagementSystem
```

Then run:

```bash
InventoryManagementSystem
```

## Project Structure

```text
Inventory-Management-System/
│
├── Inventory Management system.cpp
├── Inventory Management system.exe
└── README.md
```

## Main System Flow

```text
Start Program
     │
     ▼
Login Gateway
     │
     ├───────────────┐
     ▼               ▼
   Admin           Customer
     │               │
     ▼               ▼
Inventory         Browse Products
Management            │
     │                ▼
     │              Cart
     │                │
     │                ▼
     │             Checkout
     │                │
     └───────┬────────┘
             ▼
        Data Storage
```

## Technologies Used

- **Language:** C++
- **Programming Paradigm:** Object-Oriented Programming
- **Data Storage:** Text File Handling
- **Application Type:** Console Application

## Disclaimer

This project was developed for educational and demonstration purposes to practice C++ programming, Object-Oriented Programming, data structures, and file handling.
