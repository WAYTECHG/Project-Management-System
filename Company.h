#ifndef COMPANY_H
#define COMPANY_H

#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ============================================================================
// Abstract Base Class: Company
// Represents a generic company associated with a project..
// ============================================================================
class Company {
protected:
    int companyID;
    string companyName;
    string companyType;
    int projectID;
    string contactPerson;

public:
    // Virtual destructor for safe polymorphic deletion
    virtual ~Company() = default;

    // Constructor to initialize all member variables
    Company(int id, const string& name, const string& type, int projectID, const string& contact)
        : companyID(id), companyName(name), companyType(type), projectID(projectID), contactPerson(contact) {}

    //Getter Functions
    int getCompanyID() const { return companyID; }
    string getCompanyName() const { return companyName; }
    string getCompanyType() const { return companyType; }
    int getProjectID() const { return projectID; }
    string getContactPerson() const { return contactPerson; }

    // Setter Functions
    void setCompanyName(const string& name) { companyName = name; }
    void setCompanyType(const string& type) { companyType = type; }
    void setContactPerson(const string& contact) { contactPerson = contact; }

    // Pure virtual method that must be implemented by derived classes
    virtual void display() const = 0;

    // Generates the next unique ID by finding the current max and adding 1
    static int getNextID(const vector<Company*>& companies) {
        int maxID = 0;
        for (const Company* c : companies)
            maxID = max(maxID, c->getCompanyID());
        return maxID + 1;
    }
};

#endif
