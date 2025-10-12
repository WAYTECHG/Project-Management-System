#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "Company.h"
#include "Utils.h"
#include "Project.h"

using namespace std;

class Client : public Company {
private:
    string projectName;

public:
    Client() : Company(0, "", "", 0, "") {}  // Default constructor chains to base

    // Parameterized constructor initializes base Company class
    Client(int id, const string& name, const string& type, int projectID, const string& contact)
        : Company(id, name, type, projectID, contact) {}

    void setProjectName(const string& pname) { projectName = pname; }
    // Overridden display shows client-specific information
    void display() const override {
        cout << "Client ID    : " << companyID << "\n"
             << "Company Name  : " << companyName << "\n"
             << "Company Type  : " << companyType << "\n"
             << "Contact      : " << contactPerson << "\n"
             << "Project      : "
             << (!projectName.empty() ? projectName + " [ID: " + to_string(projectID) + "]" : "None") << "\n";
    }

    // Saves all clients to CSV with proper quoting for text fields
    static void saveClients(const vector<Client*>& clients, const string& filename = "clients.csv") {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error saving clients.\n";
            return;
        }

        file << "ClientID,Company Name,Company Type,ProjectID,Contact\n";
        for (const Client* c : clients) {
            file << c->getCompanyID() << ","
                 << '"' << c->getCompanyName() << "\","  // Quote names to handle commas
                 << '"' << c->getCompanyType() << "\","  // Quote types to handle commas
                 // Handling for unassigned projects (shows "None" instead of -1)
                 << (c->getProjectID() >= 0 ? to_string(c->getProjectID()) : "None") << ","
                 << '"' << c->getContactPerson() << "\"\n";  // Quote contact info
        }
        file.close();
    }

    // Loads clients from CS
    static void loadClients(vector<Client*>& clients, const string& filename = "clients.csv") {
        // Clear existing clients to prevent memory leaks
        for (Client* c : clients) delete c;
        clients.clear();

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error loading clients from " << filename << ".\n";
            return;
        }

        string line;
        getline(file, line); // Skip header row

        while (getline(file, line)) {
            vector<string> fields = Utils::parseCSVRow(line);  // Custom CSV parsing
            if (fields.size() != 5) {  // Validate correct number of fields
                cerr << "Malformed line: " << line << "\n";
                continue;
            }

            try {
                // Trim and process all fields
                string idStr = Utils::trim(fields[0]);
                string name = Utils::trim(fields[1]);
                string type = Utils::trim(fields[2]);
                string projIDStr = Utils::trim(fields[3]);
                string contact = Utils::trim(fields[4]);

                int id = stoi(idStr);  // Convert ID string to int
                // Handle "None" project ID case (-1 means no project assigned)
                int pid = (Utils::toLower(projIDStr) != "none") ? stoi(projIDStr) : -1;

                clients.push_back(new Client(id, name, type, pid, contact));
            } catch (const exception& e) {
                cerr << "Skipping client row due to error: " << e.what() << "\nRow: " << line << endl;
            }
        }

        file.close();
    }

    // Add new client that will be associated to a specific project
    static void addClient(vector<Client*>& clients, const vector<Project*>& projects) {
        try {
            if (projects.empty()) {  // Must have projects to assign
                cout << "No projects available. Add a project first.\n";
                return;
            }

            // Display available projects for selection
            cout << "\nSelect project for this client:\n";
            for (size_t i = 0; i < projects.size(); ++i)
                cout << i + 1 << ". [ID: " << projects[i]->getID() << "] " << projects[i]->getProjectName() << "\n";

            // Get validated project selection
            int pIndex = Utils::getValidIndexSelection(projects.size(), "project");
            int selectedProjID = projects[pIndex]->getID();

            // Get required client details
            string name = Utils::getNonEmptyInput("Enter company name: ");
            string type = Utils::getNonEmptyInput("Enter company type: ");
            string contact = Utils::getValidPhoneNumber("Enter contact number: ");

            // Generate new ID and create client
            int newID = Company::getNextID(vector<Company*>(clients.begin(), clients.end()));
            clients.push_back(new Client(newID, name, type, selectedProjID, contact));
            saveClients(clients);
            cout << "Client added.\n";

        } catch (const exception& e) {
            cerr << "Exception while adding client: " << e.what() << "\n";
        }
    }

    // Helper for selecting a client from the list
    Client* chooseClient(vector<Client*>& clients, const string& prompt) {
        if (clients.empty()) {
            return nullptr;  // Handle empty client list
        }

        cout << prompt;
        // Display all clients with index numbers
        for (size_t i = 0; i < clients.size(); ++i)
            cout << i + 1 << ". [ID: " << clients[i]->getCompanyID() << "] " << clients[i]->getCompanyName() << "\n";

        // Get validated user selection
        int index = Utils::getValidIndexSelection(clients.size(), "client");
        return clients[index];
    }

    // Update Client Details
    void updateClient(vector<Client*>& clients) {
        try {
            string choice;
            while (true) {
                cout << "\nUpdate:\n1. Name\n2. Type\n3. Contact\n0. Done\nChoice: ";
                getline(cin, choice);

                if (choice == "1") {
                    setCompanyName(Utils::getNonEmptyInput("Enter new name: "));
                } else if (choice == "2") {
                    setCompanyType(Utils::getNonEmptyInput("Enter new type: "));
                } else if (choice == "3") {
                    // Uses phone number validation for contact updates
                    setContactPerson(Utils::getValidPhoneNumber("Enter new contact number: "));
                } else if (choice == "0") {
                    saveClients(clients);  // Persist changes
                    cout << "Client updated.\n";
                    break;
                } else {
                    cout << "Invalid choice.\n";
                }
            }
        } catch (const exception& e) {
            cerr << "Exception while updating client: " << e.what() << "\n";
        }
    }

    // Client deletion
    void deleteClient(vector<Client*>& clients) {
        try {
            while (true) {
                cout << "\nAre you sure you want to delete client: [" << getCompanyID()
                    << "] " << getCompanyName() << "? (yes/no): ";

                string confirmation;
                getline(cin, confirmation);
                confirmation = Utils::toLower(Utils::whitespace(confirmation));

                if (confirmation == "yes") {
                    // Find exact position in vector
                    int idx = -1;
                    for (size_t i = 0; i < clients.size(); ++i) {
                        if (clients[i]->getCompanyID() == getCompanyID()) {
                            idx = static_cast<int>(i);
                            break;
                        }
                    }

                    if (idx != -1) {
                        delete clients[idx];  // Free memory
                        clients.erase(clients.begin() + idx);  // Remove from vector
                        saveClients(clients);  // Update storage
                        cout << "Client deleted successfully.\n";
                    } else {
                        cerr << "Error: Client not found.\n";
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
            cerr << "Exception while deleting client: " << e.what() << "\n";
        }
    }

    // Main client management menu
    void handleClientMenu(vector<Client*>& clients, const vector<Project*>& projects) {
        loadClients(clients);  // Refresh data from file
        string input;

        while (true) {
            try {
                cout << "\n=== Client Menu ===\n";
                cout << "1. Add Client\n2. Update Client\n3. Delete Client\n4. Back\nChoice: ";
                getline(cin, input);

                if (input == "1") addClient(clients, projects);
                else if (input == "2") {
                    Client* c = chooseClient(clients, "\nSelect a client to update:\n");
                    if (c != nullptr) {
                        c->updateClient(clients);  // Delegates to instance method
                    } else {
                        cout << "No clients available.\n";
                    }
                }
                else if (input == "3") {
                    Client* c = chooseClient(clients, "Select a client to delete.\n");
                    if (c != nullptr) {
                        c->deleteClient(clients);  // Delegates to instance method
                    } else {
                        cout << "No clients available.\n";
                    }
                }
                else if (input == "4") break;
                else cout << "Invalid option.\n";

            } catch (const exception& e) {
                cerr << "Exception in client menu: " << e.what() << "\n";
            }
        }
    }
};

#endif
