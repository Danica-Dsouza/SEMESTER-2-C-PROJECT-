# Community Help Desk and Resource Allocation System

**A Console-Based Management System in C**

This project is a comprehensive **Community Help Desk and Resource Allocation System** developed in **C** for managing humanitarian aid, charity, or community support operations.

---

## 📋 Project Description

A full-featured management system that helps NGOs, community centers, and relief organizations efficiently handle:

- Beneficiaries (aid recipients)
- Volunteers
- Resource/Inventory
- Resource Allocation & Distribution
- Reports and Analytics

**Course**: Programming in C (DBT7288)

---

## ✨ Features

### Core Modules

- **Beneficiary Management**
  - Add, View, Search, Edit, Delete beneficiaries
  - Stores name, contact, address, family size, need category & priority

- **Volunteer Management**
  - Manage volunteers with skills and availability status

- **Resource / Inventory Management**
  - Track items (Food, Medicine, Clothing, etc.)
  - Low stock alerts and reorder level management

- **Resource Allocation**
  - Allocate resources to beneficiaries
  - Automatic stock deduction
  - Records failed allocations due to insufficient stock

- **Search System**
  - Search beneficiaries and volunteers by ID
  - Search resources by name or category (case-insensitive)

- **Reports**
  - Stock Summary
  - Low Stock Alert
  - Beneficiaries Served
  - Pending / Failed Allocations
  - Volunteer Summary

- **Data Persistence**
  - Save & Load data from text files
  - Data survives program restarts

---

## 🛠️ Technical Details

- **Language**: C (Standard C)
- **Memory Management**: Dynamic arrays using `malloc()`, `realloc()`, and `free()`
- **File Handling**: Persistent storage using `.txt` files
- **No external libraries** – Pure C
- **Input Validation** with user-friendly messages
- **Modular Design** with clean code structure

---

## 📁 Files

- `project4.c` → Main source code
- `beneficiaries.txt` → Beneficiary records
- `volunteers.txt` → Volunteer records
- `resources.txt` → Inventory data
- `allocations.txt` → Allocation history

---

## 🚀 How to Compile and Run

### 1. Compile
```bash
gcc project4.c -o helpdesk

Sample Data Included
The program comes with realistic sample data including:

6 Beneficiaries
4 Volunteers
5 Resource items
3 Allocation records


Skills Demonstrated

Dynamic arrays with realloc()
File I/O operations
String manipulation and parsing
Modular programming & code organization
Data validation and error handling
Menu-driven interface design


Future Enhancements (Possible)

Export reports to CSV
Search by multiple criteria
Assignment of volunteers to beneficiaries
Graphical User Interface (GUI) version
Login system for admin/volunteer roles


Made with ❤️ for community service management
