#ifndef TEAMMEMBER_H
#define TEAMMEMBER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "Utils.h"
#include "Project.h"
#include "Task.h"

using namespace std;

class TeamMember {
private:
    int memberID;
    string memberName;
    string role;
    int assignedTaskID;
    string contactNumber;
    int projectID;

public:
    // Default constructor -> empty team member
    TeamMember() : memberID(0), assignedTaskID(-1), projectID(-1) {}

    // Constructor
    TeamMember(int id, const string& name, const string& role, int taskID, int projID, const string& contact)
        : memberID(id), memberName(name), role(role), assignedTaskID(taskID), contactNumber(contact), projectID(projID) {}

    // Getters
    int getMemberID() const { return memberID; }
    string getMemberName() const { return memberName; }
    string getRole() const { return role; }
    int getAssignedTaskID() const { return assignedTaskID; }
    string getContactNumber() const { return contactNumber; }
    int getProjectID() const { return projectID; }

    // Setters
    void setProjectID(int pid) { projectID = pid; }
    void setMemberName(const string& name) { memberName = name; }
    void setRole(const string& r) { role = r; }
    void setAssignedTaskID(int id) { assignedTaskID = id; }
    void setContactNumber(const string& number) { contactNumber = number; }

    // Generates the next unique ID by finding the current max and adding 1
    static int getNextID(const vector<TeamMember*>& members) {
        int maxID = 0;
        for (const TeamMember* m : members)
            maxID = max(maxID, m->getMemberID());
        return maxID + 1;
    }

    // Display team member details
    void display(const vector<Task*>& tasks, const vector<Project*>& projects) const {
        string taskName = "None";
        string projectName = "None";

        // Try to find task name from task ID
        if (assignedTaskID >= 0) {
            for (const Task* t : tasks) {
                if (t->getTaskID() == assignedTaskID) {
                    taskName = t->getTaskName() + " [ID: " + to_string(assignedTaskID) + "]";
                    break;
                }
            }
        }

        // Try to find project name from project ID
        if (projectID >= 0) {
            for (const Project* p : projects) {
                if (p->getID() == projectID) {
                    projectName = p->getProjectName() + " [ID: " + to_string(projectID) + "]";
                    break;
                }
            }
        }

        // Print member info
        cout << "ID        : " << memberID << "\n"
             << "Name      : " << memberName << "\n"
             << "Role      : " << role << "\n"
             << "Contact   : " << contactNumber << "\n"
             << "Task      : " << taskName << "\n"
             << "Project   : " << projectName << "\n";
    }

    // Save all team members details to CSV file
    static void saveMembers(const vector<TeamMember*>& members, const string& filename = "teammembers.csv") {
        try {
            ofstream file(filename);
            if (!file) throw runtime_error("Failed to open file for saving team members.");

            // Write CSV header
            file << "ID,Member Name,Role,Contact,AssignedTaskID,ProjectID\n";

            // Write each member's data as a CSV row
            for (const TeamMember* m : members) {
                file << m->getMemberID() << ","
                     << '"' << m->getMemberName() << "\","
                     << '"' << m->getRole() << "\","
                     << '"' << m->getContactNumber() << "\","
                     << (m->getAssignedTaskID() >= 0 ? to_string(m->getAssignedTaskID()) : "None") << ","
                     << (m->getProjectID() >= 0 ? to_string(m->getProjectID()) : "None") << "\n";
            }

            file.close();
        } catch (const exception& e) {
            cerr << "Exception while saving members: " << e.what() << endl;
        }
    }

    // Load team members from CSV file
    static void loadMembers(vector<TeamMember*>& members, const string& filename = "teammembers.csv") {
        // Clear any existing members
        for (TeamMember* m : members) delete m;
        members.clear();

        ifstream file(filename);
        if (!file) {
            cerr << "Failed to load team members from " << filename << ".\n";
            return;
        }

        string line;
        getline(file, line); // Skip header

        // Read and parse each line
        while (getline(file, line)) {
            vector<string> fields = Utils::parseCSVRow(line);

            // Validate CSV column count
            if (fields.size() != 6) {
                cerr << "Malformed line in teammembers.csv: " << line << endl;
                continue;
            }

            try {
                // Extract and clean each field
                string idStr = Utils::trim(fields[0]);
                string name = Utils::trim(fields[1]);
                string role = Utils::trim(fields[2]);
                string contact = Utils::trim(fields[3]);
                string taskIDStr = Utils::trim(fields[4]);
                string projectIDStr = Utils::trim(fields[5]);

                // Convert numeric fields
                int id = stoi(idStr);
                int taskID = (Utils::toLower(taskIDStr) != "none" && !taskIDStr.empty()) ? stoi(taskIDStr) : -1;
                int projID = (Utils::toLower(projectIDStr) != "none" && !projectIDStr.empty()) ? stoi(projectIDStr) : -1;

                // Add member to list
                members.push_back(new TeamMember(id, name, role, taskID, projID, contact));
            } catch (const exception& e) {
                cerr << "Skipping line due to error: " << e.what() << "\nLine: " << line << endl;
            }
        }

        file.close();
    }

    // Add a new team member to CSV file
    static void addMember(vector<TeamMember*>& members, const vector<Project*>& projects) {
        try {
            // Gather member info from user input
            string name = Utils::getNonEmptyInput("Enter member name: ");
            string role = Utils::getNonEmptyInput("Enter role: ");
            string contact = Utils::getValidPhoneNumber("Enter contact number: ");
            int id = getNextID(members);
            int taskID = -1;
            int projectID = -1;

            // Let user select a project (if any exist)
            if (projects.empty()) {
                cout << "No projects available. Project will remain unassigned.\n";
            } else {
                cout << "\nAvailable Projects:\n";
                for (size_t i = 0; i < projects.size(); ++i) {
                    cout << i + 1 << ". [" << projects[i]->getID() << "] "
                         << projects[i]->getProjectName() << "\n";
                }

                int selectedIndex = Utils::getValidIndexSelection(projects.size(), "project to assign");
                projectID = projects[selectedIndex]->getID();
            }

            // Create and save new member
            members.push_back(new TeamMember(id, name, role, taskID, projectID, contact));
            saveMembers(members);
            cout << "Team member added successfully and linked to project ID "
                 << (projectID >= 0 ? to_string(projectID) : "None") << ".\n";
        } catch (const exception& e) {
            cerr << "Error adding member: " << e.what() << endl;
        }
    }

    // Utility to choose a member from list
    TeamMember* chooseMembers(vector<TeamMember*> &members, const string &prompt){
        if(members.empty()){
            return nullptr;
        }else{
            cout << prompt;
            for (size_t i = 0; i < members.size(); ++i) {
                cout << i + 1 << ". [" << members[i]->getMemberID() << "] " << members[i]->getMemberName() << "\n";
            }
            int idx = Utils::getValidIndexSelection(members.size(), "member to update");
            return members[idx];
        }
    }

    // Update fields of a selected member
    void updateMember(TeamMember* m, vector<TeamMember*>& members, vector<Task*>& tasks, const vector<Project*>& projects) {
        try {
            string choice;
            do {
                cout << "\nWhat do you want to update?\n"
                     << "1. Name\n"
                     << "2. Role\n"
                     << "3. Contact Number\n"
                     << "4. Assigned Task\n"
                     << "5. Done\n"
                     << "Choice: ";
                getline(cin, choice);

                if (choice == "1") {
                    m->setMemberName(Utils::getNonEmptyInput("Enter new name: "));
                } else if (choice == "2") {
                    m->setRole(Utils::getNonEmptyInput("Enter new role: "));
                } else if (choice == "3") {
                    string newContact = Utils::getValidPhoneNumber("Enter new contact number: ");
                    m->setContactNumber(newContact);
                } else if (choice == "4") {
                    // Nested menu for selecting task type
                    while (true) {
                        cout << "\nSelect Task Type to assign:\n";
                        cout << "1. Main Task\n";
                        cout << "2. Sub Task\n";
                        cout << "3. Back\n";
                        cout << "Enter choice: ";
                        string typeChoice;
                        getline(cin, typeChoice);
                        typeChoice = Utils::whitespace(typeChoice);

                        if (typeChoice == "3") {
                            cout << "Returning to previous menu.\n";
                            break;
                        }

                        string type = (typeChoice == "1") ? "Main" :
                                      (typeChoice == "2") ? "Sub" : "";

                        if (type.empty()) {
                            cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                            continue;
                        }

                        // Filter tasks by selected type
                        vector<Task*> filtered;
                        for (Task* t : tasks) {
                            if (t->getType() == type) filtered.push_back(t);
                        }

                        if (filtered.empty()) {
                            cout << "No " << type << " tasks available.\n";
                            continue;
                        }

                        cout << "\nAvailable " << type << " Tasks:\n";
                        for (size_t i = 0; i < filtered.size(); ++i) {
                            string projectName = "Unknown";
                            for (const Project* p : projects) {
                                if (p->getID() == filtered[i]->getProjectID()) {
                                    projectName = p->getProjectName();
                                    break;
                                }
                            }

                            cout << i + 1 << ". " << "Task->" << filtered[i]->getTaskName()
                                 << " [ID: " << filtered[i]->getTaskID() << "], "
                                 << "Project->" << projectName << " [ID: " << filtered[i]->getProjectID() << "]\n";
                        }
                        cout << "0. Back\n";
                        cout << "Enter your choice: ";

                        string input;
                        getline(cin, input);
                        input = Utils::whitespace(input);

                        if (input == "0") {
                            cout << "Returning to task type selection.\n";
                            continue;
                        }

                        if (!all_of(input.begin(), input.end(), ::isdigit)) {
                            cout << "Invalid input. Please enter a number.\n";
                            continue;
                        }

                        int index = stoi(input) - 1;
                        if (index < 0 || static_cast<size_t>(index) >= filtered.size()) {
                            cout << "Invalid task selection.\n";
                            continue;
                        }

                        // Set selected task and project
                        int tid = filtered[index]->getTaskID();
                        int pid = filtered[index]->getProjectID();
                        m->setAssignedTaskID(tid);
                        m->setProjectID(pid);

                        cout << "Assigned " << type << " task [ID: " << tid << "] from project [ID: " << pid << "] to member.\n";
                        break;
                    }
                } else if (choice != "5") {
                    cout << "Invalid choice.\n";
                }
            } while (choice != "5");

            saveMembers(members);
            cout << "Member updated.\n";
        } catch (const exception& e) {
            cerr << "Error during member update: " << e.what() << endl;
        }
    }

    // Utility to assign a task to a team member (returns selected member pointer)
    TeamMember* assignMembers(vector<Task*> tasks, vector<TeamMember*>& members, const string& prompt) {
        if (members.empty()) {
            cout << "No team members found.\n";
            return nullptr;
        } else if (tasks.empty()) {
            cout << "No tasks available to assign.\n";
            return nullptr;
        } else {
            cout << prompt;
            for (size_t i = 0; i < members.size(); ++i) {
                cout << i + 1 << ". [" << members[i]->getMemberID() << "] " << members[i]->getMemberName() << "\n";
            }

            // Ask user to choose a member
            int idx = Utils::getValidIndexSelection(members.size(), "member to update");
            TeamMember* m = members[idx];
            return m;
        }
    }

    // Assign a member to either a Main or Sub task
    void assignToTask(vector<TeamMember*>& members, const vector<Task*>& tasks, const vector<Project*>& projects) {
        try {
            TeamMember* m = assignMembers(tasks, members, "\nSelect a team member to assign:\n");
            if (m == nullptr) return;

            while (true) {
                cout << "\nSelect Task Type to Assign:\n";
                cout << "1. Assign to Main Task\n";
                cout << "2. Assign to Sub Task\n";
                cout << "3. Back\n";
                cout << "Enter choice: ";

                string input;
                getline(cin, input);
                input = Utils::whitespace(input);

                if (input == "3") {
                    cout << "Assignment cancelled. Returning...\n\n";
                    return;
                }

                string selectedType;
                if (input == "1") selectedType = "Main";
                else if (input == "2") selectedType = "Sub";
                else {
                    cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                    continue;
                }

                // Filter tasks by type (Main or Sub)
                vector<Task*> filteredTasks;
                for (Task* t : tasks) {
                    if (t->getType() == selectedType)
                        filteredTasks.push_back(t);
                }

                if (filteredTasks.empty()) {
                    cout << "No " << selectedType << " tasks available.\n";
                    continue;
                }

                // Display list of available filtered tasks
                cout << "\nAvailable " << selectedType << " tasks:\n";
                for (size_t i = 0; i < filteredTasks.size(); ++i) {
                    string projectName = "Unknown";
                    for (const Project* p : projects) {
                        if (p->getID() == filteredTasks[i]->getProjectID()) {
                            projectName = p->getProjectName();
                            break;
                        }
                    }

                    cout << i + 1 << ". " << "Task->" << filteredTasks[i]->getTaskName()
                         << " [ID: " << filteredTasks[i]->getTaskID() << "], "
                         << "Project->" << projectName << " [ID: " << filteredTasks[i]->getProjectID() << "]\n";
                }

                cout << "0. Back\n";
                cout << "Enter your choice: ";
                getline(cin, input);
                input = Utils::whitespace(input);

                if (input == "0") {
                    cout << "Returning to task type menu.\n";
                    continue;
                }

                if (input.empty() || !all_of(input.begin(), input.end(), ::isdigit)) {
                    cout << "Invalid input. Please enter a valid number.\n\n";
                    continue;
                }

                int taskIndex = stoi(input) - 1;
                if (taskIndex < 0 || static_cast<size_t>(taskIndex) >= filteredTasks.size()) {
                    cout << "Invalid selection. No task assigned.\n\n";
                    continue;
                }

                // Update member's task and project IDs
                int selectedTaskID = filteredTasks[taskIndex]->getTaskID();
                int selectedProjectID = filteredTasks[taskIndex]->getProjectID();

                m->setAssignedTaskID(selectedTaskID);
                m->setProjectID(selectedProjectID);

                saveMembers(members);

                cout << "Assigned " << selectedType << " task [" << selectedTaskID << "] from project ["
                     << selectedProjectID << "] to " << m->getMemberName() << ".\n\n";
                return; // Exit after successful assignment
            }

        } catch (const exception& e) {
            cerr << "Error assigning task to member: " << e.what() << endl;
        }
    }

    // Delete a selected team member from the system
    void deleteMember(TeamMember* m, vector<TeamMember*>& members) {
        try {
            while (true) {
                cout << "\nAre you sure you want to delete member: ["
                     << m->getMemberID() << "] " << m->getMemberName() << "? (yes/no): ";

                string confirmation;
                getline(cin, confirmation);
                confirmation = Utils::toLower(Utils::whitespace(confirmation));

                if (confirmation == "yes") {
                    int index = -1;

                    // Find the index of the member to delete
                    for (size_t i = 0; i < members.size(); ++i) {
                        if (members[i]->getMemberID() == m->getMemberID()) {
                            index = i;
                            break;
                        }
                    }

                    if (index != -1) {
                        delete members[index]; // Free memory
                        members.erase(members.begin() + index); // Remove from vector
                        saveMembers(members); // Save updated list
                        cout << "Member deleted successfully.\n";
                    } else {
                        cerr << "Error: Member not found in the list.\n";
                    }

                    break;
                } else if (confirmation == "no") {
                    cout << "Deletion cancelled.\n";
                    break;
                } else {
                    cout << "Invalid input. Please type 'yes' or 'no'.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Error deleting member: " << e.what() << endl;
        }
    }

    // Menu to manage team members (add, update, delete)
    void handleMenu(vector<TeamMember*>& members, vector<Project*>& projects, vector<Task*>& tasks) {
        try {
            loadMembers(members); // Load members from CSV on entry

            string choice;
            while (true) {
                cout << "\n=== Team Member Menu ===\n";
                cout << "1. Add Member\n2. Update Member\n3. Delete Member\n4. Back\nChoice: ";
                getline(cin, choice);

                if (choice == "1") {
                    addMember(members, projects);
                } else if (choice == "2") {
                    TeamMember* m = chooseMembers(members, "\nSelect a member to update:\n");
                    if (m != nullptr) {
                        updateMember(m, members, tasks, projects);
                    } else {
                        cout << "\nThere are no team members available" << endl;
                    }
                } else if (choice == "3") {
                    TeamMember* m = chooseMembers(members, "\nSelect a member to delete:\n");
                    if (m != nullptr) {
                        deleteMember(m, members);
                    } else {
                        cout << "\nThere are no team members available" << endl;
                    }
                } else if (choice == "4") {
                    break; // Exit the menu
                } else {
                    cout << "Invalid choice.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Error in team member menu: " << e.what() << endl;
        }
    }
};

#endif

