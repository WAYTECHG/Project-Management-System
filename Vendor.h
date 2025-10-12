#ifndef VENDOR_H
#define VENDOR_H

#include "Company.h"
#include "Project.h"
#include "Task.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Vendor inherits from Company
class Vendor : public Company {
private:
    int taskID;
    string projectName;
    string taskName;

public:
    // Default constructor initializes base Company and sets taskID to -1 (unassigned)
    Vendor() : Company(0, "", "", 0, ""), taskID(-1) {}

    // Parameterized constructor
    Vendor(int id, const string& name, const string& type, int projectID, const string& contact, int taskID)
        : Company(id, name, type, projectID, contact), taskID(taskID) {}

    // Getter for assigned task ID
    int getTaskID() const { return taskID; }

    // Setter for task ID
    void setTaskID(int tid) { taskID = tid; }

    // Setters for cached display names
    void setProjectName(const string& pname) { projectName = pname; }
    void setTaskName(const string& tname) { taskName = tname; }

    // Getters for cached display names
    string getProjectName() const { return projectName; }
    string getTaskName() const { return taskName; }

    // Display vendor details in formatted output
    void display() const override {
        cout << "Vendor ID    : " << companyID << "\n"
             << "Company Name : " << companyName << "\n"
             << "Company Type : " << companyType << "\n"
             << "Contact      : " << contactPerson << "\n"
             << "Project      : "
             << (!projectName.empty() ? projectName + " [ID: " + to_string(projectID) + "]" : "None") << "\n"
             << "Task         : "
             << (!taskName.empty() ? taskName + " [ID: " + to_string(taskID) + "]" : "None") << "\n";
    }

    // Save list of vendors to CSV file
    static void saveVendors(const vector<Vendor*>& vendors, const string& filename = "vendors.csv") {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error saving vendors.\n";
            return;
        }

        // Write header
        file << "VendorID,Company Name,Company Type,ProjectID,Contact,TaskID\n";
        for (const Vendor* v : vendors) {
            file << v->getCompanyID() << ","
                 << '"' << v->getCompanyName() << "\","    // Quoted for safety with spaces
                 << '"' << v->getCompanyType() << "\","
                 << v->getProjectID() << ","
                 << '"' << v->getContactPerson() << "\","
                 << (v->getTaskID() >= 0 ? to_string(v->getTaskID()) : "None") << "\n";
        }

        file.close();
    }

    // Load vendors from CSV file into memory
    static void loadVendors(vector<Vendor*>& vendors, const string& filename = "vendors.csv") {
        // Clear old memory
        for (Vendor* v : vendors) delete v;
        vendors.clear();

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error loading vendors from " << filename << ".\n";
            return;
        }

        string line;
        getline(file, line); // Skip CSV header

        // Parse each line into fields
        while (getline(file, line)) {
            vector<string> fields = Utils::parseCSVRow(line);
            if (fields.size() != 6) {
                cerr << "Malformed vendor row: " << line << endl;
                continue;
            }

            try {
                // Clean and convert each field
                string idStr = Utils::trim(fields[0]);
                string name = Utils::trim(fields[1]);
                string type = Utils::trim(fields[2]);
                string projStr = Utils::trim(fields[3]);
                string contact = Utils::trim(fields[4]);
                string taskStr = Utils::trim(fields[5]);

                int id = stoi(idStr);
                int pid = stoi(projStr);
                int tid = (Utils::toLower(taskStr) != "none") ? stoi(taskStr) : -1;

                vendors.push_back(new Vendor(id, name, type, pid, contact, tid));
            } catch (const exception& e) {
                cerr << "Error parsing vendor row: " << e.what() << "\nRow: " << line << endl;
            }
        }

        file.close();
    }

    // Add a new vendor (interactive)
    static void addVendor(vector<Vendor*>& vendors, const vector<Project*>& projects) {
        try {
            if (projects.empty()) {
                cout << "No projects found. Add a project first.\n";
                return;
            }

            // Show available projects
            cout << "\nSelect project for this vendor:\n";
            for (size_t i = 0; i < projects.size(); ++i)
                cout << i + 1 << ". [ID: " << projects[i]->getID() << "] " << projects[i]->getProjectName() << "\n";

            // Get user selection
            int pIndex = Utils::getValidIndexSelection(projects.size(), "project");
            int selectedProjID = projects[pIndex]->getID();

            // Gather input from user
            string name = Utils::getNonEmptyInput("Enter company name: ");
            string type = Utils::getNonEmptyInput("Enter company type: ");
            string contact = Utils::getValidPhoneNumber("Enter contact number: ");
            int taskID = -1;  // No task assigned yet

            // Generate next unique company ID
            vector<Company*> tempCompanyList;
            for (Vendor* v : vendors)
                tempCompanyList.push_back(static_cast<Company*>(v));
            int newID = Company::getNextID(tempCompanyList);

            // Create and add vendor
            vendors.push_back(new Vendor(newID, name, type, selectedProjID, contact, taskID));
            saveVendors(vendors);
            cout << "Vendor added.\n";

        } catch (const exception& e) {
            cerr << "Error while adding vendor: " << e.what() << "\n";
        }
    }

    // Assign this vendor to a task (Main or Sub)
    void assignToTask(const vector<Task*>& tasks) {
        // Filter tasks that belong to this vendor's project
        vector<Task*> filteredTasks;
        for (Task* t : tasks) {
            if (t->getProjectID() == this->getProjectID()) {
                filteredTasks.push_back(t);
            }
        }

        if (filteredTasks.empty()) {
            cout << "No tasks available for this vendor's project.\n";
            return;
        }

        while (true) {
            // Let user pick type of task to assign
            cout << "\nAssign task to vendor: " << getCompanyName() << "\n";
            cout << "1. Assign Main Task\n";
            cout << "2. Assign Sub Task\n";
            cout << "3. Back\n";
            cout << "Choice: ";

            string choice;
            getline(cin, choice);
            choice = Utils::whitespace(choice);

            string selectedType;
            if (choice == "1") selectedType = "Main";
            else if (choice == "2") selectedType = "Sub";
            else if (choice == "3") return;
            else {
                cout << "Invalid choice. Please enter 1, 2, or 3.\n";
                continue;
            }

            // Filter tasks by type (Main/Sub)
            vector<Task*> typeFiltered;
            for (Task* t : filteredTasks) {
                if (t->getType() == selectedType) {
                    typeFiltered.push_back(t);
                }
            }

            if (typeFiltered.empty()) {
                cout << "No " << selectedType << " tasks available for this project.\n";
                continue;
            }

            // Show task choices
            cout << "\nAvailable " << selectedType << " tasks:\n";
            for (size_t i = 0; i < typeFiltered.size(); ++i) {
                cout << i + 1 << ". [ID: " << typeFiltered[i]->getTaskID() << "] "
                     << typeFiltered[i]->getTaskName() << "\n";
            }

            // Get user selection
            cout << "Enter your choice: ";
            string input;
            getline(cin, input);
            input = Utils::whitespace(input);

            if (!all_of(input.begin(), input.end(), ::isdigit)) {
                cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            int index = stoi(input) - 1;
            if (index < 0 || static_cast<size_t>(index) >= typeFiltered.size()) {
                cout << "Invalid task selection.\n";
                continue;
            }

            // Assign task to vendor
            int selectedTaskID = typeFiltered[index]->getTaskID();
            this->setTaskID(selectedTaskID);
            cout << "Vendor assigned to " << selectedType << " task successfully.\n";
            break;
        }
    }

    // Helper to choose a vendor from the list
    Vendor* chooseVendor(vector<Vendor*> &vendors, const string &prompt){
        if(vendors.empty()){
            return nullptr;
        }else{
            cout << prompt;
            for (size_t i = 0; i < vendors.size(); ++i)
                cout << i + 1 << ". [ID: " << vendors[i]->getCompanyID() << "] " << vendors[i]->getCompanyName() << "\n";

            int index = Utils::getValidIndexSelection(vendors.size(), "vendor");
            return vendors[index];
        }
    }

    // Update specific fields of a vendor
    void updateVendor(Vendor *v, vector<Vendor*>& vendors) {
        try {
            string choice;
            while (true) {
                cout << "\nUpdate:\n1. Name\n2. Type\n3. Contact\n0. Done\nChoice: ";
                getline(cin, choice);

                if (choice == "1") v->setCompanyName(Utils::getNonEmptyInput("Enter new name: "));
                else if (choice == "2") v->setCompanyType(Utils::getNonEmptyInput("Enter new type: "));
                else if (choice == "3") v->setContactPerson(Utils::getValidPhoneNumber("Enter new contact number: "));
                else if (choice == "0") {
                    saveVendors(vendors);
                    cout << "Vendor updated.\n";
                    break;
                } else {
                    cout << "Invalid option.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Error while updating vendor: " << e.what() << "\n";
        }
    }

    // Delete vendor with confirmation
    void deleteVendor(Vendor* v, vector<Vendor*>& vendors) {
        try {
            while (true) {
                cout << "\nAre you sure you want to delete vendor: [" << v->getCompanyID()
                     << "] " << v->getCompanyName() << "? (yes/no): ";
                string confirmation;
                getline(cin, confirmation);
                confirmation = Utils::toLower(Utils::whitespace(confirmation));

                // Delete vendor if user prompts "yes"
                if (confirmation == "yes") {
                    auto it = find_if(vendors.begin(), vendors.end(), [&](Vendor* x) {
                        return x->getCompanyID() == v->getCompanyID();
                    });

                    if (it != vendors.end()) {
                        delete *it;
                        vendors.erase(it);
                        saveVendors(vendors);
                        cout << "Vendor deleted successfully.\n";
                    } else {
                        cerr << "Error: Vendor not found.\n";
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
            cerr << "Error while deleting vendor: " << e.what() << "\n";
        }
    }

    // Vendor menu for user interaction
    void handleVendorMenu(vector<Vendor*>& vendors, const vector<Project*>& projects, const vector<Task*>& tasks) {
        loadVendors(vendors);
        string input;

        while (true) {
            try {
                cout << "\n=== Vendor Menu ===\n";
                cout << "1. Add Vendor\n";
                cout << "2. Assign Tasks to vendor\n";
                cout << "3. Update Vendor\n";
                cout << "4. Delete Vendor\n";
                cout << "5. Back\nChoice: ";
                getline(cin, input);

                if (input == "1") addVendor(vendors, projects);
                else if (input == "2"){
                    Vendor* v = chooseVendor(vendors, "Select vendor to assign task:\n");
                    if (v) {
                        v->assignToTask(tasks);
                        saveVendors(vendors);
                    } else cout << "No vendors available.\n";
                }
                else if (input == "3") {
                    Vendor *v = chooseVendor(vendors, "\nSelect a vendor to update\n");
                    if (v) updateVendor(v, vendors);
                    else cout << "\nThere are no vendors available" << endl;
                }
                else if (input == "4") {
                    Vendor *v = chooseVendor(vendors, "Select a vendor to delete.\n");
                    if (v) deleteVendor(v, vendors);
                    else cout << "\nThere are no vendors available" << endl;
                }
                else if (input == "5") break;
                else cout << "Invalid option.\n";

            } catch (const exception& e) {
                cerr << "Menu error: " << e.what() << "\n";
            }
        }
    }
};

#endif
