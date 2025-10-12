#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Task.h"
#include "Project.h"
#include "Utils.h"

using namespace std;

// TaskManager class handles everything related to tasks:
// saving/loading from file, adding new tasks, updating details, and deleting tasks.
class TaskManager{
public:
    // Default constructor
    TaskManager() = default;

    // Save all tasks (Main + Sub) details to a CSV file
    static void saveTasks(const vector<Task*>& tasks, const string& filename = "tasks.csv") {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open " << filename << " for writing.\n";
            return;
        }

        // Write header
        file << "TaskID,Type,Task Name,Description,StartDate,EndDate,Status,ProjectID,ParentTaskID\n";
        // Write each task line
        for (const Task* t : tasks) {
            file << t->getTaskID() << ","
                << t->getType() << ","
                << "\"" << t->getTaskName() << "\"," // Quote to handle commas
                << "\"" << t->getDescription() << "\","
                << t->getStartDate() << ","
                << t->getEndDate() << ","
                << t->getStatus() << ","
                << t->getProjectID() << ","
                << (t->getParentTaskID() == -1 ? "None" : to_string(t->getParentTaskID())) // Handle "no parent" case
                << "\n";
        }

        file.close();
    }

    // Load tasks from CSV file and store them into the task list
    static void loadTasks(vector<Task*>& tasks, const string& filename = "tasks.csv") {
        // Clean up existing tasks to prevent memory leaks
        for (Task* t : tasks) delete t;
        tasks.clear();

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open " << filename << " for reading.\n";
            return;
        }

        string line;
        getline(file, line); // Skip the CSV header

        // Read and reconstruct each task line by line
        while (getline(file, line)) {
            vector<string> fields = Utils::parseCSVRow(line);

            // Validate correct number of fields
            if (fields.size() != 9) {
                cerr << "Skipping malformed task row: " << line << "\n";
                continue;
            }

            try {
                // Parse and trim all fields
                string idStr = Utils::trim(fields[0]);
                string type = Utils::trim(fields[1]);
                string name = Utils::trim(fields[2]);
                string desc = Utils::trim(fields[3]);
                string start = Utils::trim(fields[4]);
                string end = Utils::trim(fields[5]);
                string status = Utils::trim(fields[6]);
                string projStr = Utils::trim(fields[7]);
                string parentStr = Utils::trim(fields[8]);

                if (idStr.empty() || projStr.empty())
                    throw invalid_argument("Missing Task ID or Project ID");

                // Convert string IDs to integers
                int id = stoi(idStr);
                int projID = stoi(projStr);
                // Handle "None" parent case (-1 means no parent)
                int parentID = (parentStr.empty() || parentStr == "None") ? -1 : stoi(parentStr);

                // Create appropriate task type
                if (type == "Main") {
                    tasks.push_back(new MainTask(id, name, desc, start, end, status, projID));
                } else if (type == "Sub") {
                    tasks.push_back(new SubTask(id, name, desc, start, end, status, projID, parentID));
                } else {
                    cerr << "Unknown task type in row: " << line << ". Skipping.\n";
                }

            } catch (const exception& e) {
                cerr << "Skipping task row due to error: " << e.what() << "\nRow: " << line << endl;
            }
        }

        file.close();
    }

    // Generates the next unique ID by finding the current max and adding 1
    static int getNextID(const vector<Task*>& tasks) {
        int maxID = 0;
        for (const Task* t : tasks)
            maxID = max(maxID, t->getTaskID());
        return maxID + 1;
    }

    // Handles adding both Main and Sub Tasks with project/date validation
    static void addTask(vector<Task*>& tasks, const vector<Project*>& projects) {
        try {
            if (projects.empty()) {
                cout << "No projects available. Add a project first.\n";
                return;
            }

            // Let user choose the project to add task into
            cout << "\nSelect project for this task:\n";
            for (size_t i = 0; i < projects.size(); ++i) {
                cout << i + 1 << ". [ID: " << projects[i]->getID() << "] " << projects[i]->getProjectName() << "\n";
            }

            int projIndex = Utils::getValidIndexSelection(projects.size(), "project");
            Project* selectedProject = projects[projIndex];
            int projID = selectedProject->getID();

            // Let user choose the type of task to add
            string choice;
            while (true) {
                cout << "\nSelect Task Type:\n";
                cout << "1. Add Main Task\n";
                cout << "2. Add Sub Task\n";
                cout << "3. Back\n";
                cout << "Choice: ";
                getline(cin, choice);

                if (choice == "1") {
                    // Adding a Main Task
                    string name = Utils::getNonEmptyInput("Enter main task name: ");

                    // Check for duplicate task names in same project
                    for (const Task* t : tasks) {
                        if (t->getTaskName() == name && t->getProjectID() == projID) {
                            cout << "Task already exists in this project.\n";
                            return;
                        }
                    }

                    string desc = Utils::getNonEmptyInput("Enter description: ");
                    string start, end;

                    // Ensure dates fall within project range
                    while (true) {
                        start = Utils::getValidDate("Start date");
                        end = Utils::getValidDate("End date");

                        if (!Utils::isDateWithinProjectRange(start, end, selectedProject->getStartDate(), selectedProject->getEndDate())) {
                            cout << "Dates must be within project range: "
                                << selectedProject->getStartDate() << " - " << selectedProject->getEndDate() << "\n";
                            continue;
                        }
                        break;
                    }

                    string status = Utils::getStatusUser();
                    // Create and save new task
                    int newID = TaskManager::getNextID(tasks);
                    tasks.push_back(new MainTask(newID, name, desc, start, end, status, projID));
                    TaskManager::saveTasks(tasks);
                    cout << "Main task added successfully.\n";
                    return;

                } else if (choice == "2") {
                    // Adding a Sub Task
                    // Make sure at least one Main Task exists under this project
                    vector<Task*> mainTasks;
                    for (Task* t : tasks) {
                        if (t->getType() == "Main" && t->getProjectID() == projID)
                            mainTasks.push_back(t);
                    }

                    if (mainTasks.empty()) {
                        cout << "No Main Tasks found for this project. Add one first.\n";
                        continue;
                    }

                    // Show available parent main tasks
                    cout << "\nSelect Parent Main Task:\n";
                    for (size_t i = 0; i < mainTasks.size(); ++i) {
                        cout << i + 1 << ". [ID: " << mainTasks[i]->getTaskID() << "] " << mainTasks[i]->getTaskName() << endl;
                    }

                    int mainIndex = Utils::getValidIndexSelection(mainTasks.size(), "main task");
                    Task* parent = mainTasks[mainIndex];
                    int parentID = parent->getTaskID();

                    // Collect details for sub-task
                    string name = Utils::getNonEmptyInput("Enter sub task name: ");
                    for (const Task* t : tasks) {
                        if (t->getTaskName() == name && t->getProjectID() == projID) {
                            cout << "Task already exists in this project.\n";
                            return;
                        }
                    }

                    string desc = Utils::getNonEmptyInput("Enter description: ");
                    string start, end;

                    // Validate sub task dates within both main task and project
                    while (true) {
                        start = Utils::getValidDate("Start date");
                        end = Utils::getValidDate("End date");

                        if (!Utils::isDateWithinRange(start, end, selectedProject->getStartDate(), selectedProject->getEndDate())) {
                            cout << "Dates must be within project range: "
                                << selectedProject->getStartDate() << " - " << selectedProject->getEndDate() << "\n";
                            continue;
                        }

                        if (!Utils::isDateWithinRange(start, end, parent->getStartDate(), parent->getEndDate())) {
                            cout << "Dates must be within parent task range: "
                                << parent->getStartDate() << " - " << parent->getEndDate() << "\n";
                            continue;
                        }

                        break;
                    }

                    // Create and save new sub-task
                    string status = Utils::getStatusUser();
                    int newID = TaskManager::getNextID(tasks);

                    tasks.push_back(new SubTask(newID, name, desc, start, end, status, projID, parentID));
                    TaskManager::saveTasks(tasks);
                    cout << "Sub task added successfully.\n";
                    return;

                } else if (choice == "3") {
                    cout << "Returning to previous menu.\n";
                    return;
                } else {
                    cout << "Invalid input. Please enter 1, 2, or 3.\n";
                }
            }

        } catch (const exception& e) {
            cerr << "Error adding task: " << e.what() << "\n";
        }
    }

    // Update an existing task's information (name, description, date, status)
    void updateTask(vector<Task*>& tasks, const vector<Project*>& projects) {
        if (projects.empty()) {
            cout << "No projects available.\n";
            return;
        }

        // Let user pick a project to filter down the tasks
        cout << "\nSelect a project:\n";
        for (size_t i = 0; i < projects.size(); ++i) {
            cout << i + 1 << ". [" << projects[i]->getID() << "] " << projects[i]->getProjectName() << "\n";
        }
        int projIndex = Utils::getValidIndexSelection(projects.size(), "project");
        int selectedProjectID = projects[projIndex]->getID();

        while (true) {
            // Ask user if they want to update a Main Task or Sub Task
            cout << "\n=== Update Task Menu ===\n";
            cout << "1. Update Main Task\n";
            cout << "2. Update Sub Task\n";
            cout << "3. Back\n";
            cout << "Choice: ";

            string typeChoice;
            getline(cin, typeChoice);

            if (typeChoice == "1" || typeChoice == "2") {
                string type = (typeChoice == "1") ? "Main" : "Sub";

                // Filter tasks by selected project and type
                vector<Task*> filtered;
                for (Task* t : tasks) {
                    if (t->getType() == type && t->getProjectID() == selectedProjectID) {
                        filtered.push_back(t);
                    }
                }

                if (filtered.empty()) {
                    cout << "No " << type << " tasks available in this project.\n";
                    continue;
                }

                // Let user choose which task to update
                cout << "\nSelect a " << type << " task to update:\n";
                for (size_t i = 0; i < filtered.size(); ++i) {
                    cout << i + 1 << ". [ID: " << filtered[i]->getTaskID() << "] " << filtered[i]->getTaskName() << "\n";
                }

                int index = Utils::getValidIndexSelection(filtered.size(), type + " task");
                Task* t = filtered[index];

                // Now allow user to edit specific fields
                while (true) {
                    cout << "\n1. Update Name\n"
                        << "2. Update Description\n"
                        << "3. Update Start Date\n"
                        << "4. Update End Date\n"
                        << "5. Update Status\n"
                        << "6. Done\n"
                        << "Choice: ";
                    string choice;
                    getline(cin, choice);

                    if (choice == "1") {
                        t->setTaskName(Utils::getNonEmptyInput("New name: "));

                    } else if (choice == "2") {
                        t->setDescription(Utils::getNonEmptyInput("New description: "));

                    } else if (choice == "3" || choice == "4") {
                        // Handle start or end date update
                        string newDate = Utils::getValidDate(choice == "3" ? "New start date" : "New end date");
                        string otherDate = (choice == "3") ? t->getEndDate() : t->getStartDate();

                        string start = (choice == "3") ? newDate : otherDate;
                        string end = (choice == "4") ? newDate : otherDate;

                        if (!Utils::isValidDateRange(start, end)) {
                            cout << "Invalid date range. Start must be before or equal to end.\n";
                            continue;
                        }

                        // Get associated project
                        Project* proj = nullptr;
                        for (Project* p : projects) {
                            if (p->getID() == t->getProjectID()) {
                                proj = p;
                                break;
                            }
                        }

                        if (!proj) {
                            cerr << "Associated project not found.\n";
                            continue;
                        }

                        // Validate date is within project range
                        if (!Utils::isDateWithinRange(start, end, proj->getStartDate(), proj->getEndDate())) {
                            cout << "Dates must be within project range: " << proj->getStartDate() << " - " << proj->getEndDate() << "\n";
                            continue;
                        }

                        // If it's a subtask, also validate against its parent main task
                        if (t->getType() == "Sub") {
                            Task* parent = nullptr;
                            for (Task* task : tasks) {
                                if (task->getTaskID() == t->getParentTaskID()) {
                                    parent = task;
                                    break;
                                }
                            }

                            if (!parent) {
                                cerr << "Parent Main Task not found.\n";
                                continue;
                            }

                            if (!Utils::isDateWithinRange(start, end, parent->getStartDate(), parent->getEndDate())) {
                                cout << "Dates must be within parent task range: "
                                    << parent->getStartDate() << " - " << parent->getEndDate() << "\n";
                                continue;
                            }
                        }

                        // Apply the change
                        if (choice == "3") t->setStartDate(newDate);
                        if (choice == "4") t->setEndDate(newDate);

                    } else if (choice == "5") {
                        // Let user select a new status (Not Started, In Progress, Completed)
                        t->setStatus(Utils::getStatusUser());

                    } else if (choice == "6") {
                        // Finish updating
                        break;

                    } else {
                        cout << "Invalid choice.\n";
                    }
                }

                TaskManager::saveTasks(tasks);
                cout << "Task updated.\n";
                break;

            } else if (typeChoice == "3") {
                break;

            } else {
                cout << "Invalid choice. Please select 1-3.\n";
            }
        }
    }

    // Update only the status field of a specific task
    void updateTaskStatus(Task* t, vector<Task*>& tasks) {
        if (!t) return;

        try {
            string input;
            while (true) {
                cout << "\nChoose new status:\n";
                cout << "1. Not Started\n2. In Progress\n3. Completed\nChoice: ";
                getline(cin, input);

                input = Utils::whitespace(input);
                if (input == "1") {
                    t->setStatus("Not Started");
                    break;
                } else if (input == "2") {
                    t->setStatus("In Progress");
                    break;
                } else if (input == "3") {
                    t->setStatus("Completed");
                    break;
                } else {
                    cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                }
            }

            saveTasks(tasks);
            cout << "Task status updated.\n";

        } catch (const exception& e) {
            cerr << "Error: " << e.what() << "\nStatus not updated.\n";
        }
    }

    // Delete a task (either main or sub). If main task is deleted, all its sub-tasks are also removed.
    int deleteTask(vector<Task*>& tasks, const vector<Project*>& projects) {
        if (projects.empty()) {
            cout << "No projects available.\n";
            return -1;
        }

        // Choose the project from which to delete
        cout << "\nSelect a project:\n";
        for (size_t i = 0; i < projects.size(); ++i) {
            cout << i + 1 << ". [" << projects[i]->getID() << "] " << projects[i]->getProjectName() << "\n";
        }
        int projIndex = Utils::getValidIndexSelection(projects.size(), "project");
        int selectedProjectID = projects[projIndex]->getID();

        while (true) {
            // Ask whether to delete a main or sub task
            cout << "\n=== Delete Task Menu ===\n";
            cout << "1. Delete Main Task\n";
            cout << "2. Delete Sub Task\n";
            cout << "3. Back\n";
            cout << "Choice: ";

            string choice;
            getline(cin, choice);

            if (choice == "1" || choice == "2") {
                string type = (choice == "1") ? "Main" : "Sub";

                // Filter tasks of that type
                vector<Task*> filtered;
                for (Task* t : tasks) {
                    if (t->getType() == type && t->getProjectID() == selectedProjectID) {
                        filtered.push_back(t);
                    }
                }

                if (filtered.empty()) {
                    cout << "No " << type << " tasks found in this project.\n";
                    continue;
                }

                // Select task to delete
                cout << "\nSelect a " << type << " task to delete:\n";
                for (size_t i = 0; i < filtered.size(); ++i) {
                    cout << i + 1 << ". [ID: " << filtered[i]->getTaskID() << "] " << filtered[i]->getTaskName() << "\n";
                }

                int taskIndex = Utils::getValidIndexSelection(filtered.size(), type + " task");
                Task* t = filtered[taskIndex];
                int deleteID = t->getTaskID();

                // Confirm deletion
                cout << "\nAre you sure you want to delete task: [ID " << deleteID
                    << "] " << t->getTaskName() << "? (yes/no): ";
                string confirmation;
                getline(cin, confirmation);
                confirmation = Utils::toLower(Utils::whitespace(confirmation));

                if (confirmation == "yes") {
                    // Delete the selected task
                    int index = -1;
                    for (size_t i = 0; i < tasks.size(); ++i) {
                        if (tasks[i]->getTaskID() == deleteID) {
                            index = i;
                            break;
                        }
                    }

                    if (index == -1) {
                        cerr << "Error: Task not found.\n";
                        return -1;
                    }

                    delete tasks[index];
                    tasks.erase(tasks.begin() + index);

                    // If it's a main task, remove its sub-tasks too
                    if (type == "Main") {
                        auto it = remove_if(tasks.begin(), tasks.end(), [&](Task* task) {
                            SubTask* sub = dynamic_cast<SubTask*>(task);
                            if (sub && sub->getParentTaskID() == deleteID && sub->getProjectID() == selectedProjectID) {
                                delete sub;
                                return true;
                            }
                            return false;
                        });
                        tasks.erase(it, tasks.end());

                        cout << "Main task and all its sub-tasks have been deleted.\n";
                    } else {
                        cout << "Sub-task deleted.\n";
                    }

                    TaskManager::saveTasks(tasks);
                    return deleteID;

                } else if (confirmation == "no") {
                    cout << "Deletion cancelled.\n";
                    continue;
                } else {
                    cout << "Invalid input. Please type 'yes' or 'no'.\n";
                }

            } else if (choice == "3") {
                return -1; // back to previous menu
            } else {
                cout << "Invalid choice. Please enter 1-3.\n";
            }
        }
    }

    // Menu to let the user choose to Add, Update, or Delete tasks
    int handleMenu(vector<Task*>& tasks, const vector<Project*>& projects){
        string choice;
        int deletedID = -1;

        try {
            while (true) {
                cout << "\n=== Task Menu ===\n";
                cout << "1. Add Task\n2. Update Task\n3. Delete Task\n4. Back\nChoice: ";
                getline(cin, choice);

                if (choice == "1") addTask(tasks, projects);
                else if (choice == "2"){
                    updateTask(tasks, projects);
                }
                else if (choice == "3") {
                    deletedID = deleteTask(tasks, projects);
                } else if (choice == "4") break;
                else cout << "Invalid input. Try again.\n";
            }
        } catch (const exception& e) {
            cerr << "Unhandled exception in Task menu: " << e.what() << "\n";
        }

        return deletedID; // Returns ID of deleted task (if any)
    }
};

#endif
