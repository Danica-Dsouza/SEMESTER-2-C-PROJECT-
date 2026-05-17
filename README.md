# Community Help Desk and Resource Allocation System
### C Version — project4.c
**Course:** DBT7288 – Programming in C
**Institution:** Symbiosis International University Dubai

---

## Overview

A menu-driven console application written in C that manages a community aid organization's operations. It handles beneficiary registration, volunteer coordination, resource inventory, and resource allocation — with all data saved to and loaded from text files between sessions.

---

## Features

- **Beneficiary Management** — Add, view, search, edit, and delete beneficiary records
- **Volunteer Management** — Add, view, search, edit, and delete volunteer records
- **Resource Inventory** — Add, view, search (by name or category), edit, and delete resource items
- **Resource Allocation** — Allocate resources to beneficiaries with automatic stock deduction; records failed allocations when stock is insufficient
- **Search Records** — Global search across beneficiaries (by ID), volunteers (by ID), and resources (by name/category keyword)
- **Reports**
  - Stock Summary
  - Low Stock Alert
  - Beneficiaries Served
  - Pending / Failed Allocations
  - Volunteer Summary
- **File Persistence** — Auto-saves on exit; loads existing data on startup
- **Sample Data** — Option to seed 15+ demo records on first run

---

## File Structure

```
project4.c              # Main source file (all modules in one file)
beneficiaries.txt       # Saved beneficiary records
volunteers.txt          # Saved volunteer records
resources.txt           # Saved resource records
allocations.txt         # Saved allocation records
```

---

## Data Structures

| Struct | Fields |
|---|---|
| `Beneficiary` | ID, name, contact, address, need category, family size, priority level |
| `Volunteer` | ID, name, phone, skill, availability |
| `Resource` | Item ID, name, category, quantity available, reorder level |
| `Allocation` | Allocation ID, beneficiary ID, item ID, requested qty, allocated qty, date |

---

## How to Compile and Run

### Using GCC (Linux / macOS / WSL)
```bash
gcc project4.c -o project4
./project4
```

### Using GCC (Windows Command Prompt)
```bash
gcc project4.c -o project4.exe
project4.exe
```

### Using VS Code
1. Open the folder containing `project4.c`
2. Install the **C/C++ extension** by Microsoft
3. Press `Ctrl+Shift+B` to build, then run via the terminal

---

## How to Use

1. On first launch, choose `1` to load sample data (15+ records)
2. Navigate using the numbered main menu
3. All sub-menus follow the same pattern — enter the number for your action
4. Enter `0` in any sub-menu to go back
5. Choose option `8` (Exit) to save and quit — data is also auto-saved on exit

---

## Key Concepts Used

- Structs (`typedef struct`)
- Dynamic memory allocation (`malloc`, `realloc`, `free`)
- Pointer arithmetic and double pointers
- File I/O (`fopen`, `fprintf`, `fscanf`, `fgets`, `fclose`)
- Modular function design
- Input validation with re-prompting loops
- String handling (`strncpy`, `strcspn`, `strstr`, `tolower`)
- Pipe-delimited file format with a count header per file

---

## File Format (pipe-delimited)

Each file begins with a record count on the first line, followed by one record per line:

```
3
1001|Aisha Al Mansoori|0501234567|Al Ain, Block 3|Food|4|1
1002|Ravi Kumar|0559876543|Mussafah, Villa 7|Medicine|2|2
1003|Sara Qasim|0523456789|Sharjah, Apt 12|Clothing|6|3
```

---

## Known Limitations

- No confirmation prompt before deleting a record
- Search for beneficiaries and volunteers is by ID only (no name search)
- `strncpy` does not guarantee null-termination if input exactly fills the buffer
- `allocateResource()` uses `malloc + memcpy + free` instead of `realloc` (minor inconsistency)
- No undo functionality

---

## Priority Levels

| Level | Label |
|---|---|
| 1 | Urgent |
| 2 | Normal |
| 3 | Low |
