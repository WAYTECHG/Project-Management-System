#include <iostream>
#include "ProjectManagementSystem.h"

using namespace std;

// Displays the main menu options to the user
void displayMenu() {
    cout << "\n=== Project Management Application ===" << endl;
    cout << "1. Add, update, and delete projects" << endl;
    cout << "2. Add, update, and delete tasks" << endl;
    cout << "3. Add, update, and delete team members" << endl;
    cout << "4. Add, update, assign task, and delete vendors" << endl;
    cout << "5. Add, update, and delete clients" << endl;
    cout << "6. Assign tasks to team members" << endl;
    cout << "7. Update task status" << endl;
    cout << "8. Display all project, task, team member, vendor, client details" << endl;
    cout << "9. Search projects and tasks by status" << endl;
    cout << "10. View overdue tasks and projects" << endl;
    cout << "0. Exit Program" << endl;
}

int main() {
    try {
        // Create the central management system (loads all data from CSVs)
        ProjectManagementSystem system;

        string choice;

        // Main application loop
        while (true) {
            displayMenu();
            cout << "Enter choice: ";
            getline(cin, choice);

            // Project management menu
            if (choice == "1") {
                Project p;
                int deletedID = p.handleProjectMenu(system.getProjects());

                // If a project was deleted, remove all related data
                if (deletedID != -1) {
                    system.removeOrphanedProjectData(deletedID);
                }

            // Task management menu
            } else if (choice == "2") {
                system.handleTasks();

            // Team member management menu
            } else if (choice == "3") {
                TeamMember m;
                m.handleMenu(system.getTeamMembers(), system.getProjects(), system.getTasks());

            // Vendor management menu
            } else if (choice == "4") {
                Vendor v;
                v.handleVendorMenu(system.getVendors(), system.getProjects(), system.getTasks());
                system.refreshCompanies(); // Update unified company list

            // Client management menu
            } else if (choice == "5") {
                Client c;
                c.handleClientMenu(system.getClients(), system.getProjects());
                system.refreshCompanies(); // Update unified company list

            // Assign task to team members
            } else if (choice == "6") {
                system.assignTaskToMember();

            // Update task status (main/sub)
            } else if (choice == "7") {
                system.updateTaskStatusFromMenu();

            // Display data (full detail menu)
            } else if (choice == "8") {
                system.displayEntityMenu();

            // Search by project/task status
            } else if (choice == "9") {
                system.searchStatus();

            // Display overdue projects and tasks
            } else if (choice == "10") {
                system.displayOverdueItems();

            // Exit program
            } else if (choice == "0") {
                cout << "Exiting Project Management Application. Goodbye!\n";
                break;

            // Invalid input
            } else {
                cout << "Invalid choice. Please enter a number from 0 to 10.\n";
            }
        }

    } catch (const exception& e) {
        // Exception safety for fatal errors (e.g., file errors, allocation failure)
        cerr << "\nUnexpected error occurred: " << e.what() << "\n";
        cerr << "The application encountered a fatal error and needs to close.\n";
        return 1;
    }

    return 0;
}
