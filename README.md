# 🏗️ Project Management System (C++)

## 📘 Overview

The **Project Management System** is a console-based C++ application designed to efficiently manage all aspects of a project — including **Projects**, **Tasks (Main & Sub)**, **Team Members**, **Vendors**, and **Clients**.
It follows **Object-Oriented Programming (OOP)** principles such as **inheritance**, **polymorphism**, **encapsulation**, and **abstraction**, and uses **STL containers**, **file I/O (CSV)**, and **exception handling** for data management and persistence.

This project is designed for educational and practical use, demonstrating how complex systems can be modularized and structured in C++.

---

## 🚀 Features

### 🔹 Core Functionalities

- **Project Management** — Add, update, delete, and view projects.
- **Task Management** — Create, update, and delete main and sub-tasks with deadlines and status tracking.
- **Team Member Management** — Add, update, delete, and assign tasks to team members.
- **Vendor Management** — Manage vendors, assign them to projects, update details, and delete records.
- **Client Management** — Handle client information and project associations.
- **Task Assignment** — Assign tasks to specific team members.
- **Task Status Updates** — Update task progress (Pending, In Progress, Completed).
- **Comprehensive Display Menu** — View all data (projects, tasks, members, vendors, clients) in organized tables.
- **Search by Status** — Filter projects and tasks by their current status.
- **Overdue Alerts** — Identify and display overdue projects and tasks automatically.
- **File Persistence (CSV)** — All data (projects, tasks, vendors, clients, team members) are saved to and loaded from CSV files on startup and exit.

---

## 🧩 System Structure

### **Main Components**

| Class                               | Description                                                                                               |
| ----------------------------------- | --------------------------------------------------------------------------------------------------------- |
| `ProjectManagementSystem`         | Central system controller that integrates all modules (Project, TaskManager, Vendor, Client, TeamMember). |
| `Project`                         | Manages project data (name, deadline, status) with CSV persistence.                                       |
| `Task`, `MainTask`, `SubTask` | Handle main and sub-tasks using polymorphism and date validation.                                         |
| `TaskManager`                     | Manages task creation, updating, deletion, and association with projects.                                 |
| `TeamMember`                      | Handles member information and task assignments.                                                          |
| `Vendor`                          | Inherits from `Company`; manages vendor details and project relations.                                  |
| `Client`                          | Inherits from `Company`; manages client details and related projects.                                   |
| `Company`                         | Abstract base class for `Vendor` and `Client` (uses polymorphism).                                    |
| `Utils`                           | Contains input validation, formatting, and date utilities.                                                |

---



## 💾 Data Persistence

All entities (Projects, Tasks, Members, Vendors, Clients) are stored in **CSV files**, making data easily readable and editable outside the program.Each module includes:

- `loadFromCSV()` — Loads saved data on startup.
- `saveToCSV()` — Saves updated data when changes occur or on exit.

---

## 🧠 Object-Oriented Design Highlights

- **Polymorphism**: Used in `Company`, `Task`, and their derived classes.
- **Inheritance**: `Vendor` and `Client` inherit from `Company`.
- **Encapsulation**: Private member variables with getters/setters.
- **Abstraction**: Abstract base classes like `Company` enforce interface design.
- **Static Methods**: Used for generating unique IDs (e.g., `Company::getNextID()`).
- **Exception Handling**: Ensures safe program termination and error messages on unexpected issues.

---

## 🛠️ Technologies Used

- **Language**: C++
- **Libraries**: Standard Template Library (STL)
- `<vector>` for dynamic data storage
- `<string>` for text handling
- `<algorithm>` for sorting/searching
- `<fstream>` for CSV file I/O
- **File Format**: `.csv` for persistence
- **Compiler**: GCC / MinGW / any C++17-compatible compiler

---

## 🧩 Example Code Structure

/ProjectManagementSystem
│
├── main.cpp
├── ProjectManagementSystem.h
├── Project.h
├── TaskManager.h
├── Task.h
├── TeamMember.h
├── Vendor.h
├── Client.h
├── Company.h
├── Utils.h
└── data/
├── projects.csv
├── tasks.csv
├── team_members.csv
├── vendors.csv
└── clients.csv

---



## 🧑‍💻 Author

**Wilbert Yonathan**

CST209 – Object-Oriented Programming (Final Project)

---

## 🏁 License

This project is released under the  **MIT License** .

You are free to use, modify, and distribute it for educational or personal purposes.

---
