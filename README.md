## 🏨 Hotel & Destination Management System (C Project)

### 📋 Overview

This project is a **menu-driven Hotel & Destination Management System** written in C.

It integrates **sorting algorithms, searching, knapsack optimization, and graph algorithms** (like Dijkstra’s shortest path) to help manage hotel and destination data efficiently.

The system allows users to:

* Add and view hotels or destinations
* Sort hotels using Bubble Sort or Merge Sort
* Search hotels using Binary Search
* Find the best hotel combination under a given budget (Knapsack)
* Manage paths between destinations
* Calculate shortest travel routes using Dijkstra’s algorithm
* Access extended analytical features in the “Extension” module

---

### ⚙️ Features

| Function                           | Description                                                                       |
| ---------------------------------- | --------------------------------------------------------------------------------- |
| **Add Hotel**                | Add a new hotel with name, price, rating, and distance.                           |
| **View All Hotels**          | Display the complete list of hotels.                                              |
| **Bubble Sort / Merge Sort** | Sort hotels by price, rating, or distance.                                        |
| **Binary Search**            | Search for a hotel by ID or name.                                                 |
| **Knapsack**                 | Recommend hotels that fit within a user’s budget.                                |
| **Add Destination**          | Add new travel destinations.                                                      |
| **Add Path**                 | Define paths (connections) between destinations.                                  |
| **Dijkstra Algorithm**       | Find the shortest route between two destinations.                                 |
| **Extension Menu**           | Access advanced features such as matrix operations or combined sorting/filtering. |

---

### 🧩 Project Structure

<pre class="overflow-visible!" data-start="1765" data-end="2482"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>📁 Hotel_Management_System
│
├── main.c                </span><span># Entry point for the program</span><span>
├── hotel_func.c          </span><span># Handles hotel/destination operations and file I/O</span><span>
├── Algorithm.c           </span><span># Implements sorting, searching, knapsack, and Dijkstra</span><span>
├── Extension.c           </span><span># Contains extra/advanced algorithmic features</span><span>
│
├── hotel_func.h          </span><span># Header for hotel_func.c</span><span>
├── Algorithm.h           </span><span># Header for Algorithm.c</span><span>
├── Extension.h           </span><span># Header for Extension.c</span><span>
│
├── hotels.txt            </span><span># Stores hotel data</span><span>
├── destination.txt       </span><span># Stores destination data</span><span>
├── distances.txt         </span><span># Stores distance matrix between destinations</span><span>
│
└── README.md             </span><span># Project documentation (this file)</span><span>
</span></span></code></div></div></pre>

---

### 🧠 Key Algorithms Used

| Category               | Algorithm               | Description                                      |
| ---------------------- | ----------------------- | ------------------------------------------------ |
| **Sorting**      | Bubble Sort, Merge Sort | Organizes hotel data based on selected criteria. |
| **Searching**    | Binary Search           | Finds hotels quickly in sorted data.             |
| **Optimization** | Knapsack Problem        | Selects hotels within a budget for best value.   |
| **Graph Theory** | Dijkstra’s Algorithm   | Computes the shortest path between destinations. |

---

### 🧮 Input Validation

The program includes robust input validation functions:

* `get_ValidInt(min, max)` → Ensures valid menu choice input
* `get_ValidBudget(min, max)` → Ensures valid numeric budget input

  Invalid entries are handled gracefully with informative messages.

---



### 👨‍💻 Contributors

* **Wilbert Yonathan and Chua Kian Hoong** – Project Developer
* CST209 – Final Project (Hotel & Destination Management System)

---

### 🧾 License

This project is released for  **educational and academic use only** .

Feel free to modify or extend it for learning purposes.
