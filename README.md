# Student Record Management

A C-based project for managing student records using custom data structures, including doubly linked lists, hash tables, and inverted indexes. The program supports efficient record insertion, searching, deletion, and statistical queries.

---

## Features

- Abstract Data Structures: Lists, Hash Tables, Inverted Indexes.
- Command-line flexibility for input and configuration.
- Interactive commands for managing records and queries.

---

## Folder Structure

project/ ├── programs/ # Main program & Makefile ├── include/ # Header files ├── modules/ # Data structure implementations ├── misc/ # Input/config files

---

## Getting Started

### Build & Run
1. Build the project: `make`
2. Run the program: `./exec [options]`

### Command-Line Arguments
- **Default Mode**: `./exec`  
- **Input File**: `./exec -i input.txt`  
- **Config File**: `./exec -c config.txt`  
- **Combined**: `./exec -i input.txt -c config.txt`

---

## Interactive Commands

| Command | Description                                      |
|---------|--------------------------------------------------|
| `i`     | Insert a record.                                |
| `l`     | Look up a record by ID.                         |
| `d`     | Delete a record by ID.                          |
| `n`     | Number of students in a year.                   |
| `t`     | Top GPA students for a year.                    |
| `a`     | Average GPA for a year.                         |
| `m`     | Minimum GPA for a year.                         |
| `c`     | Student count per year.                         |
| `p`     | Rank postal codes by student count.             |
| `e`     | Exit the program.                               |

---

## Data Structures

1. **Doubly Linked List (ADTBList)**: Generic storage with bidirectional traversal.
2. **Hash Table (ADTMap)**: Key-value storage with duplicate handling.
3. **Inverted Index**: Group records by year for efficient queries.

---
