#ifndef PROJECTMANAGEMENTSYSTEM_H
#define PROJECTMANAGEMENTSYSTEM_H

#include <iostream>
#include <vector>
#include <string>
#include "Project.h"
#include "Task.h"
#include "TaskManager.h"
#include "TeamMember.h"
#include "Company.h"
#include "Vendor.h"
#include "Client.h"
#include "Utils.h"

using namespace std;

/**
 * ProjectManagementSystem
 * ------------------------
 * Central coordinator class managing projects, tasks, team members,
 * vendors, clients, and their interactions. Provides menu-driven
 * functionalities to update, display, and manage relationships.
 */

class ProjectManagementSystem {
private:
    vector<Project*> projects;
    vector<Task*> tasks;
    vector<TeamMember*> members;
    vector<Vendor*> vendors;
    vector<Client*> clients;
    vector<Company*> companies; // Unified container for both clients & vendors

    // Utility Finders
    const Project* findProjectByID(int id) const {
        for (const Project* p : projects)
            if (p->getID() == id) return p;
        return nullptr;
    }

    const Task* findTaskByID(int id) const {
        for (const Task* t : tasks)
            if (t->getTaskID() == id) return t;
        return nullptr;
    }

    // Display Assigned Members for Task
    void displayAssignedTeamMembers(const Task* t) const {
        cout << "  >> Assigned Team Member(s):\n";
        bool found = false;
        for (const TeamMember* m : members) {
            if (m->getAssignedTaskID() == t->getTaskID()) {
                cout << "     - " << m->getMemberName() << " (" << m->getRole() << ")\n";
                found = true;
            }
        }
        if (!found) cout << "     [No team members assigned]\n";
    }

    // Display Assigned Vendors for Task
    void displayAssignedVendors(const Task* t) const {
        cout << "  >> Assigned Vendor(s):\n";
        bool found = false;
        for (const Company* comp : companies) {
            const Vendor* v = dynamic_cast<const Vendor*>(comp);
            if (v && v->getProjectID() == t->getProjectID() && v->getTaskID() == t->getTaskID()) {
                cout << "     - " << v->getCompanyName() << " (" << v->getCompanyType() << ")\n";
                found = true;
            }
        }
        if (!found) cout << "     [No vendors assigned]\n";
    }

public:
    // Constructor and Destructor
    ProjectManagementSystem() {
        loadAllData(); // Load data form CSVs
    }

    ~ProjectManagementSystem() {
        // Clean up all dynamically allocated memory
        for (Project* p : projects) delete p;
        for (Task* t : tasks) delete t;
        for (TeamMember* m : members) delete m;
        for (Vendor* v : vendors) delete v;
        for (Client* c : clients) delete c;
        companies.clear(); // No need to delete again, already deleted above
    }

    // Accessors
    vector<Project*>& getProjects() { return projects; }
    vector<Task*>& getTasks() { return tasks; }
    vector<TeamMember*>& getTeamMembers() { return members; }
    vector<Vendor*>& getVendors() { return vendors; }
    vector<Client*>& getClients() { return clients; }

    // Update company list (clients + vendors)
    void refreshCompanies() {
        companies.clear();
        companies.insert(companies.end(), vendors.begin(), vendors.end());
        companies.insert(companies.end(), clients.begin(), clients.end());
    }

    // Handle Task Menu
    void handleTasks() {
        TaskManager tm;
        int deletedTaskID = tm.handleMenu(tasks, projects);

        // If a task was deleted, remove related references
        if (deletedTaskID != -1) {
            unlinkDeletedTaskReferences(deletedTaskID);
            refreshCompanies();
        }
    }

    // Update Status of Tasks (Main/Sub)
    void updateTaskStatusFromMenu() {
        if (projects.empty()) {
            cout << "No projects available.\n";
            return;
        }

        // Let user select a project
        cout << "\nSelect a project to view tasks:\n";
        for (size_t i = 0; i < projects.size(); ++i) {
            cout << i + 1 << ". " << projects[i]->getProjectName() << " [ID: " << projects[i]->getID() << "]\n";
        }

        int projectIndex = Utils::getValidIndexSelection(projects.size(), "project");
        int selectedProjectID = projects[projectIndex]->getID();

        // Ask user to choose Main or Sub task type
        while (true) {
            cout << "\nChoose Task Type to Update Status:\n";
            cout << "1. Main Task\n";
            cout << "2. Sub Task\n";
            cout << "3. Back\n";
            cout << "Choice: ";

            string choice;
            getline(cin, choice);

            // Handle Main Task Status Update
            if (choice == "1") {
                // Filter all main tasks for selected project
                vector<Task*> mainTasks;
                for (Task* t : tasks) {
                    if (t->getProjectID() == selectedProjectID && t->getType() == "Main") {
                        mainTasks.push_back(t);
                    }
                }

                if (mainTasks.empty()) {
                    cout << "No main tasks found for this project.\n";
                    continue;
                }

                // Let user choose a main task
                cout << "\nSelect a main task to update its status:\n";
                for (size_t i = 0; i < mainTasks.size(); ++i) {
                    cout << i + 1 << ". [" << mainTasks[i]->getTaskID() << "] " << mainTasks[i]->getTaskName() << "\n";
                }

                int taskIndex = Utils::getValidIndexSelection(mainTasks.size(), "main task");
                Task* selectedTask = mainTasks[taskIndex];

                TaskManager tm;
                tm.updateTaskStatus(selectedTask, tasks);
                continue;

            // Handle Sub Task Status Update
            } else if (choice == "2") {
                // First, select a Main Task (to filter subtasks)
                vector<Task*> mainTasks;
                for (Task* t : tasks) {
                    if (t->getProjectID() == selectedProjectID && t->getType() == "Main") {
                        mainTasks.push_back(t);
                    }
                }

                if (mainTasks.empty()) {
                    cout << "No main tasks found in this project. Sub tasks must belong to a main task.\n";
                    continue;
                }

                cout << "\nSelect a main task to view its subtasks:\n";
                for (size_t i = 0; i < mainTasks.size(); ++i) {
                    cout << i + 1 << ". [" << mainTasks[i]->getTaskID() << "] " << mainTasks[i]->getTaskName() << "\n";
                }

                int mainIndex = Utils::getValidIndexSelection(mainTasks.size(), "main task");
                int selectedMainID = mainTasks[mainIndex]->getTaskID();

                // Filter sub tasks under the chosen main task
                vector<Task*> subTasks;
                for (Task* t : tasks) {
                    if (t->getProjectID() == selectedProjectID &&
                        t->getType() == "Sub" &&
                        t->getParentTaskID() == selectedMainID) {
                        subTasks.push_back(t);
                    }
                }

                if (subTasks.empty()) {
                    cout << "No sub tasks found under this main task.\n";
                    continue;
                }

                // Let user choose a subtask
                cout << "\nSelect a sub task to update its status:\n";
                for (size_t i = 0; i < subTasks.size(); ++i) {
                    cout << i + 1 << ". [" << subTasks[i]->getTaskID() << "] " << subTasks[i]->getTaskName() << "\n";
                }

                int subIndex = Utils::getValidIndexSelection(subTasks.size(), "sub task");
                Task* selectedSubTask = subTasks[subIndex];

                TaskManager tm;
                tm.updateTaskStatus(selectedSubTask, tasks);
                continue;

            } else if (choice == "3") {
                cout << "Returning to main menu.\n";
                return;
            } else {
                cout << "Invalid input. Please enter 1, 2, or 3.\n";
            }
        }
    }

    // Assign tasks for team member
    void assignTaskToMember() {
        TeamMember m;
        m.assignToTask(members, tasks, projects);  // Calls logic from TeamMember
    }


    // Load All Data from CSVs
    void loadAllData() {
        // Clean previous data
        for (Project* p : projects) delete p;
        for (Task* t : tasks) delete t;
        for (TeamMember* m : members) delete m;
        for (Vendor* v : vendors) delete v;
        for (Client* c : clients) delete c;

        projects.clear();
        tasks.clear();
        members.clear();
        vendors.clear();
        clients.clear();
        companies.clear();

        // Reload from files
        Project::loadProjects(projects);
        TaskManager::loadTasks(tasks);
        TeamMember::loadMembers(members);
        Vendor::loadVendors(vendors);
        Client::loadClients(clients);

        refreshCompanies(); // Merge vendor & client into companies
    }

    // === Display Functions ===
    // Show all projects
    void displayProjects() const {
        cout << "=== Project List ===\n\n";
        if (projects.empty()) {
            cout << "No projects available.\n\n";
            return;
        }
        for (const Project* p : projects) {
            p->display();
            cout << "--------------------\n\n";
        }
    }

    // Show all tasks + assigned people
    void displayTasks() const {
        cout << "=== Task List ===\n";
        if (tasks.empty()) {
            cout << "No tasks available.\n\n";
            return;
        }
        for (const Task* t : tasks) {
            t->display(projects, tasks);

            displayAssignedTeamMembers(t);
            displayAssignedVendors(t);

            cout << "==========================================\n\n";
        }
    }

    // Show all team members
    void displayTeamMembers() const {
        cout << "=== Team Member List ===\n\n";
        if (members.empty()) {
            cout << "No team members available.\n\n";
            return;
        }
        for (const TeamMember* m : members) {
            m->display(tasks, projects);
            cout << "--------------------\n\n";
        }
    }

    // Show all vendors with linked projects/tasks
    void displayVendors() const {
        cout << "=== Vendor List ===\n\n";
        bool found = false;

        for (Company* c : companies) {
            Vendor* v = dynamic_cast<Vendor*>(c);  // remove const so we can call setProjectName
            if (v) {
                const Project* proj = findProjectByID(v->getProjectID());
                const Task* task = findTaskByID(v->getTaskID());

                // Set project and task names so that Vendor::display() shows them
                v->setProjectName(proj ? proj->getProjectName() : "");
                v->setTaskName(task ? task->getTaskName() : "");

                v->display();
                cout << "--------------------\n\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No vendors available.\n\n";
        }
    }

    // Show all clients with linked project
    void displayClients() const {
        cout << "=== Client List ===\n\n";
        bool found = false;

        for (Company* c : companies) {
            Client* cl = dynamic_cast<Client*>(c);  // remove const to allow setProjectName()
            if (cl) {
                const Project* proj = findProjectByID(cl->getProjectID());
                cl->setProjectName(proj ? proj->getProjectName() : "");

                cl->display();
                cout << "--------------------\n\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No clients available.\n\n";
        }
    }

    // Show full breakdown of one selected project
    void displaySelectedProjectDetails() {
        if (projects.empty()) {
            cout << "No projects available.\n";
            return;
        }

        cout << "\nSelect a project to display:\n";
        for (size_t i = 0; i < projects.size(); ++i) {
            cout << i + 1 << ". " << projects[i]->getProjectName()
                << " [ID: " << projects[i]->getID() << "]\n";
        }

        int index = Utils::getValidIndexSelection(projects.size(), "project");
        Project* selectedProject = projects[index];
        int selectedProjectID = selectedProject->getID();

        // === Display Project Info ===
        cout << "\n=== Project Info ===\n";
        selectedProject->display();
        cout << "--------------------\n";

        // === Display Tasks ===
        cout << "\n=== Tasks for Project ===\n";
        bool foundTask = false;
        for (const Task* t : tasks) {
            if (t->getProjectID() == selectedProjectID) {
                cout << "\n>> Task: " << t->getTaskName()
                    << " [ID: " << t->getTaskID() << "]\n";
                t->display(projects, tasks);

                displayAssignedTeamMembers(t);
                displayAssignedVendors(t);

                cout << "--------------------\n";
                foundTask = true;
            }
        }
        if (!foundTask) {
            cout << "No tasks for this project.\n";
        }

        // === Display Team Members ===
        cout << "\n=== Team Members for Project ===\n";
        bool foundMember = false;
        for (const TeamMember* m : members) {
            if (m->getProjectID() == selectedProjectID) {
                m->display(tasks, projects);  // Enhanced display with task/project names
                cout << "--------------------\n";
                foundMember = true;
            }
        }
        if (!foundMember) {
            cout << "No team members for this project.\n";
        }

        // === Display Vendors ===
        cout << "\n=== Vendors for Project ===\n";
        bool foundVendor = false;
        for (Vendor* v : vendors) {
            if (v->getProjectID() == selectedProjectID) {
                const Task* task = findTaskByID(v->getTaskID());

                v->setProjectName(selectedProject->getProjectName());
                v->setTaskName(task ? task->getTaskName() : "");

                v->display();  // Includes project/task name
                cout << "--------------------\n";
                foundVendor = true;
            }
        }
        if (!foundVendor) {
            cout << "No vendors for this project.\n";
        }

        // === Display Client ===
        cout << "\n=== Client for Project ===\n";
        bool foundClient = false;
        for (Client* c : clients) {
            if (c->getProjectID() == selectedProjectID) {
                c->setProjectName(selectedProject->getProjectName());
                c->display();  // Includes project name
                cout << "--------------------\n";
                foundClient = true;
            }
        }
        if (!foundClient) {
            cout << "No client assigned to this project.\n";
        }
    }

    // Show everything
    void displayAllDetails() {
        cout << "\n\n==============================\n";
        cout << "       PROJECT DETAILS        \n";
        cout << "==============================\n";
        displayProjects();

        cout << "\n\n==============================\n";
        cout << "         TASK DETAILS         \n";
        cout << "==============================\n";
        displayTasks();

        cout << "\n\n==============================\n";
        cout << "     TEAM MEMBER DETAILS      \n";
        cout << "==============================\n";
        displayTeamMembers();

        cout << "\n\n==============================\n";
        cout << "        VENDOR DETAILS        \n";
        cout << "==============================\n";
        displayVendors();

        cout << "\n\n==============================\n";
        cout << "        CLIENT DETAILS        \n";
        cout << "==============================\n";
        displayClients();
    }

    // Search by status (project/task)
    void searchStatus() {
        string input;

        while (true) {
            cout << "\n=== Search by Status ===\n";
            cout << "1. Search by Project Status\n";
            cout << "2. Search by Task Status\n";
            cout << "0. Cancel\n";
            cout << "Enter choice: ";
            getline(cin, input);
            input = Utils::whitespace(input);

            if (input == "0") return;

            if (input == "1" || input == "2") {
                string statusChoice;
                while (true) {
                    cout << "\nSelect Status:\n";
                    cout << "1. Not Started\n";
                    cout << "2. In Progress\n";
                    cout << "3. Completed\n";
                    cout << "0. Cancel\n";
                    cout << "Enter choice: ";
                    getline(cin, statusChoice);
                    statusChoice = Utils::whitespace(statusChoice);

                    string status;
                    if (statusChoice == "1") status = "Not Started";
                    else if (statusChoice == "2") status = "In Progress";
                    else if (statusChoice == "3") status = "Completed";
                    else if (statusChoice == "0") break;
                    else {
                        cout << "Invalid status choice. Try again.\n";
                        continue;
                    }

                    vector<Project*> projResults;
                    vector<Task*> taskResults;

                    // Collect results based on type (Project or Task)
                    if (input == "1") {
                        // Project status search (polymorphism)
                        for (const Project* p : projects) {
                            if (Utils::toLower(p->getStatus()) == Utils::toLower(status)) {
                                projResults.push_back(const_cast<Project*>(p));
                            }
                        }
                    } else {
                        // Task status search (polymorphism)
                        for (const Task* t : tasks) {
                            if (Utils::toLower(t->getStatus()) == Utils::toLower(status)) {
                                taskResults.push_back(const_cast<Task*>(t));
                            }
                        }
                    }

                    // Display Projects or Tasks based on the results
                    if (input == "1") {
                        cout << "\n=== Projects with Status: " << status << " ===\n";
                        if (projResults.empty()) {
                            cout << "No projects found with status \"" << status << "\".\n";
                        } else {
                            for (auto* p : projResults) {
                                p->display();  // Polymorphic behavior
                                cout << "--------------------------\n";
                            }
                        }
                    } else {
                        cout << "\n=== Tasks with Status: " << status << " ===\n";
                        if (taskResults.empty()) {
                            cout << "No tasks found with status \"" << status << "\".\n";
                        } else {
                            for (auto* t : taskResults) {
                                const Project* proj = findProjectByID(t->getProjectID());
                                string projName = proj ? proj->getProjectName() : "Unknown Project";

                                cout << "- Task: " << t->getTaskName()
                                    << " [ID: " << t->getTaskID() << "] | "
                                    << "Project: " << projName << " [ID: " << t->getProjectID() << "] | "
                                    << "Status: " << t->getStatus() << "\n";
                            }
                        }
                    }

                    break; // exit status loop after successful display
                }
                break; // exit outer loop after one valid operation
            } else {
                cout << "Invalid option. Please enter 1, 2, or 0.\n";
            }
        }
    }

    // Display overdue tasks/projects
    void displayOverdueItems() const {
        while (true) {
            cout << "\n=== Overdue Items Menu ===\n";
            cout << "1. View Overdue Tasks\n";
            cout << "2. View Overdue Projects\n";
            cout << "3. View Both Tasks and Projects\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";

            string choice;
            getline(cin, choice);

            string today = Utils::getTodayDate();
            bool validChoice = true;  // Flag to track whether the input is valid

            // Option 1: View overdue tasks
            if (choice == "1") {
                cout << "\n=== Overdue Tasks ===\n";
                bool found = false;
                for (const Task* t : tasks) {
                    if (Utils::compareDates(t->getDueDate(), today) < 0 &&
                        Utils::toLower(t->getStatus()) != "completed") {
                        cout << "- Task: " << t->getTaskName() << " [ID: " << t->getTaskID() << "] | "
                            << "Due: " << t->getDueDate() << " | Status: " << t->getStatus() << "\n";
                        found = true;
                    }
                }
                if (!found) cout << "No overdue tasks found.\n";
            }

            // Option 2: View overdue projects
            else if (choice == "2") {
                cout << "\n=== Overdue Projects ===\n";
                bool found = false;
                for (const Project* p : projects) {
                    if (Utils::compareDates(p->getDueDate(), today) < 0 &&
                        Utils::toLower(p->getStatus()) != "completed") {
                        cout << "- Project: " << p->getProjectName() << " [ID: " << p->getID() << "] | "
                            << "Due: " << p->getDueDate() << " | Status: " << p->getStatus() << "\n";
                        found = true;
                    }
                }
                if (!found) cout << "No overdue projects found.\n";
            }

            // Option 3: View both overdue tasks and projects
            else if (choice == "3") {
                cout << "\n=== Overdue Tasks ===\n";
                bool taskFound = false;
                for (const Task* t : tasks) {
                    if (Utils::compareDates(t->getDueDate(), today) < 0 &&
                        Utils::toLower(t->getStatus()) != "completed") {
                        cout << "- Task: " << t->getTaskName() << " [ID: " << t->getTaskID() << "] | "
                            << "Due: " << t->getDueDate() << " | Status: " << t->getStatus() << "\n";
                        taskFound = true;
                    }
                }
                if (!taskFound) cout << "No overdue tasks found.\n";

                cout << "\n=== Overdue Projects ===\n";
                bool projectFound = false;
                for (const Project* p : projects) {
                    if (Utils::compareDates(p->getDueDate(), today) < 0 &&
                        Utils::toLower(p->getStatus()) != "completed") {
                        cout << "- Project: " << p->getProjectName() << " [ID: " << p->getID() << "] | "
                            << "Due: " << p->getDueDate() << " | Status: " << p->getStatus() << "\n";
                        projectFound = true;
                    }
                }
                if (!projectFound) cout << "No overdue projects found.\n";
            }

            // Option 0: Back to main menu
            else if (choice == "0") {
                break; // Exit the loop and return to the main menu
            }

            // Invalid input
            else {
                validChoice = false;  // If the choice is invalid, set flag to false
            }

            if (!validChoice) {
                cout << "Invalid input. Please enter 0-3.\n";  // Show only when invalid input
            }
        }
    }

    // User menu for choosing what to view
    void displayEntityMenu() {
        string choice;

        while (true) {
            cout << "\n=== Display Menu ===\n";
            cout << "1. Display All Projects\n";
            cout << "2. Display All Tasks\n";
            cout << "3. Display All Team Members\n";
            cout << "4. Display All Vendors\n";
            cout << "5. Display All Clients\n";
            cout << "6. Display Full Details for One Project\n";
            cout << "7. Display ALL Available Details\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";
            getline(cin, choice);

            if (choice == "1") {
                displayProjects();
            } else if (choice == "2") {
                displayTasks();
            } else if (choice == "3") {
                displayTeamMembers();
            } else if (choice == "4") {
                displayVendors();
            } else if (choice == "5") {
                displayClients();
            } else if (choice == "6") {
                displaySelectedProjectDetails();
            } else if (choice == "7") {
                displayAllDetails();
            } else if (choice == "0") {
                break;
            } else {
                cout << "Invalid choice. Please enter a number from 0 to 7.\n";
            }
        }
    }

    // Cascade delete on project deletion
    void removeOrphanedProjectData(int deletedProjectID) {
        // --- Remove Tasks ---
        tasks.erase(remove_if(tasks.begin(), tasks.end(),
            [deletedProjectID](Task* t) {
                if (t->getProjectID() == deletedProjectID) {
                    delete t;
                    return true;
                }
                return false;
            }), tasks.end());
        TaskManager::saveTasks(tasks);

        // --- Remove Team Members ---
        members.erase(remove_if(members.begin(), members.end(),
            [deletedProjectID](TeamMember* m) {
                if (m->getProjectID() == deletedProjectID) {
                    delete m;
                    return true;
                }
                return false;
            }), members.end());
        TeamMember::saveMembers(members);

        // --- Remove Vendors ---
        vendors.erase(remove_if(vendors.begin(), vendors.end(),
            [deletedProjectID](Vendor* v) {
                if (v->getProjectID() == deletedProjectID) {
                    delete v;
                    return true;
                }
                return false;
            }), vendors.end());
        Vendor::saveVendors(vendors);

        // --- Remove Clients ---
        clients.erase(remove_if(clients.begin(), clients.end(),
            [deletedProjectID](Client* c) {
                if (c->getProjectID() == deletedProjectID) {
                    delete c;
                    return true;
                }
                return false;
            }), clients.end());
        Client::saveClients(clients);

        refreshCompanies();
    }

    // Remove dangling task references
    void unlinkDeletedTaskReferences(int deletedTaskID) {
        // === Team Members ===
        for (TeamMember* m : members) {
            if (m->getAssignedTaskID() == deletedTaskID) {
                m->setAssignedTaskID(-1);
            }
        }
        TeamMember::saveMembers(members);

        // === Vendors ===
        for (Vendor* v : vendors) {
            if (v->getTaskID() == deletedTaskID) {
                v->setTaskID(-1);
            }
        }
        Vendor::saveVendors(vendors);
    }

};

#endif
