#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "Utils.h"

using namespace std;

class Project {
private:
    int id;
    string projectName;
    string description;
    string startDate;
    string endDate;
    string status;

public:
    // Default constructor -> empty project
    Project() : id(0), projectName(""), description(""), startDate(""), endDate(""), status("") {}

    // Constructor
    Project(int id, const string& name, const string& desc, const string& start, const string& end, const string& stat)
        : id(id), projectName(name), description(desc), startDate(start), endDate(end), status(stat) {}

    virtual ~Project() {}  // Virtual destructor for proper cleanup in derived classes

    // Getter functions
    int getID() const { return id; }
    string getProjectName() const { return projectName; }
    string getDescription() const { return description; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }

    // Getting status
    virtual string getStatus() const { return status; }
    // Getting duedate
    virtual string getDueDate() const { return endDate; }

    // Setter functions
    void setProjectName(const string& name) { projectName = name; }
    void setDescription(const string& desc) { description = desc; }
    void setStartDate(const string& start) { startDate = start; }
    void setEndDate(const string& end) { endDate = end; }
    void setStatus(const string& stat) { status = stat; }

    // Displays project details
    void display() const {
        // If essential data is missing, warn the user instead of showing incomplete info
        if (projectName.empty() || status.empty()) {
            cerr << "Warning: Project data incomplete. Skipping display.\n";
            return;
        }
        cout << "ID: " << id << "\n"
             << "Name: " << projectName << "\n"
             << "Description: " << description << "\n"
             << "Start Date: " << startDate << "\n"
             << "End Date: " << endDate << "\n"
             << "Status: " << status << "\n";
    }

    // Generates the next unique ID by finding the current max and adding 1
    static int getNextID(const vector<Project*>& projects) {
        int maxID = 0;
        for (const Project* p : projects) {
            maxID = max(maxID, p->getID()); // Finds highest existing ID
        }
        return maxID + 1; // Returns next available ID
    }

    // Saves all project details to a CSV file
    static void saveProjects(const vector<Project*>& projects, const string& filename = "projects.csv") {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Unable to open file for writing projects.");
            return;
        }

        // Header row
        file << "ID,Project Name,Description,StartDate,EndDate,Status\n";
        for (const Project* p : projects) {
            // Wrap name and description in quotes to handle commas inside them
            file << p->id << ","
                 << '"' << p->projectName << "\","
                 << '"' << p->description << "\","
                 << p->startDate << ","
                 << p->endDate << ","
                 << p->status << "\n";
        }

        file.close();
    }

    // Loads project data from CSV file
    static void loadProjects(vector<Project*>& projects, const string& filename = "projects.csv") {
        // Free any existing memory and clear the list before loading
        for (Project* p : projects) delete p;
        projects.clear();

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Cannot open file to load projects.\n";
            return;
        }

        string line;
        getline(file, line); // Skip the header row

        while (getline(file, line)) {
            vector<string> fields = Utils::parseCSVRow(line);

            if (fields.size() != 6) {
                cerr << "Skipping malformed row: " << line << endl;
                continue;
            }

            try { // Trim and validate all fields before creating project
                string idStr = Utils::trim(fields[0]);
                string name = Utils::trim(fields[1]);
                string desc = Utils::trim(fields[2]);
                string start = Utils::trim(fields[3]);
                string end = Utils::trim(fields[4]);
                string status = Utils::trim(fields[5]);

                if (idStr.empty()) throw invalid_argument("Empty project ID");

                int id = stoi(idStr);  // convert ID string to int

                projects.push_back(new Project(id, name, desc, start, end, status));
            } catch (const exception& e) {
                cerr << "Skipping project row due to error: " << e.what() << "\nRow: " << line << endl;
            }
        }

        file.close();
    }

    // Prompts user to enter new project details, validates them, and saves
    static void addProject(vector<Project*>& projects) {
        try {
            string name = Utils::getNonEmptyInput("Enter project name: ");
            string desc = Utils::getNonEmptyInput("Enter description: ");

            string start, end;
            do {
                start = Utils::getValidDate("Start Date");
                end = Utils::getValidDate("End Date");
                if (!Utils::isValidDateRange(start, end)) {
                    cout << "Error: End date must be after or equal to start date.\nPlease re-enter.\n";
                }
            } while (!Utils::isValidDateRange(start, end));

            string status = Utils::getStatusUser();
            int newID = Project::getNextID(projects);

            Project* newProject = new Project(newID, name, desc, start, end, status);
            projects.push_back(newProject);

            Project::saveProjects(projects);  // Save data to file
            cout << "Project added successfully with ID: " << newID << "\n\n";
        } catch (const exception& e) {
            cerr << "Exception in addProject: " << e.what() << endl;
        }
    }

    // Allows user to choose a project from the list (returns selected pointer or nullptr)
    Project* chooseProject(vector<Project*> &projects, const string &prompt){
        if(projects.empty()){
            return nullptr;
        }else{
            cout << prompt;
            for (size_t i = 0; i < projects.size(); ++i) {
                cout << i + 1 << ". [ID " << projects[i]->getID() << "] " << projects[i]->getProjectName() << "\n";
            }

            int index = Utils::getValidIndexSelection(projects.size(), "project to update");
            Project *p = projects[index];
            return p;
        }
    }

    // Display menu update project to update project fields
    void updateProject(Project* p, vector<Project*>& projects) {
        try {
            bool updated = false;

            while (true) {
                cout << "\nWhich field would you like to update?\n";
                cout << "1. Project Name\n";
                cout << "2. Description\n";
                cout << "3. Start Date\n";
                cout << "4. End Date\n";
                cout << "5. Status\n";
                cout << "0. Finish Updating\n";
                cout << "Enter your choice: ";

                string choice;
                getline(cin, choice);
                choice = Utils::whitespace(choice);  // trim input

                if (choice == "1") {
                    p->setProjectName(Utils::getNonEmptyInput("Enter new project name: "));
                    updated = true;
                } else if (choice == "2") {
                    p->setDescription(Utils::getNonEmptyInput("Enter new description: "));
                    updated = true;
                } else if (choice == "3") {
                    string start;
                    do {
                        start = Utils::getValidDate("New Start Date");
                    } while (!Utils::isValidDateRange(start, p->getEndDate()));
                    p->setStartDate(start);
                    updated = true;
                } else if (choice == "4") {
                    string end;
                    do {
                        end = Utils::getValidDate("New End Date");
                    } while (!Utils::isValidDateRange(p->getStartDate(), end));
                    p->setEndDate(end);
                    updated = true;
                } else if (choice == "5") {
                    p->setStatus(Utils::getStatusUser());
                    updated = true;
                } else if (choice == "0") {
                    if (updated) {
                        Project::saveProjects(projects);
                        cout << "Project updated and saved.\n";
                    } else {
                        cout << "No changes made.\n";
                    }
                    break;
                } else {
                    cout << "Invalid choice.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Exception during project update: " << e.what() << endl;
        }
    }

    // Confirms and deletes selected project from memory and CSV
    int deleteProject(Project* p, vector<Project*>& projects) {
        try {
            while (true) {
                cout << "\nAre you sure you want to delete project: [ID "
                     << p->getID() << "] "
                     << p->getProjectName() << "? (yes/no): ";

                string confirmation;
                getline(cin, confirmation);
                confirmation = Utils::toLower(Utils::whitespace(confirmation));

                if (confirmation == "yes") {
                    int deletedID = p->getID();
                    string deletedName = p->getProjectName();

                    int index = -1;
                    for (size_t i = 0; i < projects.size(); i++) {
                        if (projects[i]->getID() == deletedID) {
                            index = i;
                            break;
                        }
                    }

                    if (index != -1) {
                        delete projects[index];                       // free memory
                        projects.erase(projects.begin() + index);     // remove from list
                        Project::saveProjects(projects);              // save to file

                        cout << "Deleted project [ID " << deletedID << "] " << deletedName << ".\n";
                        return deletedID;
                    } else {
                        cerr << "Error: Project not found in the list.\n";
                        return -1;
                    }

                } else if (confirmation == "no") {
                    cout << "Deletion cancelled. Returning to project list.\n";
                    return -1;
                } else {
                    cout << "Invalid input. Please type 'yes' or 'no'.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Exception during project deletion: " << e.what() << endl;
            return -1;
        }
    }

    // Displays project management menu and handles actions
    int handleProjectMenu(vector<Project*>& projects) {
        Project::loadProjects(projects);

        int deletedID = -1;

        try {
            while (true) {
                cout << "\n=== Project Management ===\n";
                cout << "1. Add Project\n";
                cout << "2. Update Project\n";
                cout << "3. Delete Project\n";
                cout << "4. Back to Main Menu\n";
                cout << "Enter choice: ";

                string choice;
                getline(cin, choice);

                // Add Project
                if (choice == "1") {
                    addProject(projects);
                } else if (choice == "2") { // Update Project
                    Project *p = chooseProject(projects, "\nSelect a project to update:\n");
                    if(p != nullptr){
                        updateProject(p, projects);
                    } else {
                        cout << "\nThere are no projects available" << endl;
                    }

                } else if (choice == "3") { // Delete Project
                    Project *p = chooseProject(projects, "\nSelect a project to delete:\n");
                    if(p != nullptr){
                        deletedID = deleteProject(p, projects);
                        return deletedID;
                    } else {
                        cout << "\nThere are no projects available" << endl;
                        return -1;
                    }
                } else if (choice == "4") { // Go back to main menu
                    break;
                } else {
                    cout << "Invalid choice.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Unexpected error in project menu: " << e.what() << endl;
        }

        return deletedID;
    }
};

#endif
