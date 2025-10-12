#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <vector>
#include <string>
#include "Project.h"

using namespace std;

// Base class for all types of tasks (MainTask or SubTask)
class Task {
protected:
    int taskID;
    string taskName, description, startDate, endDate, status;
    int projectID;  // Link task to a specific project

public:
    // Default constructor
    Task() {}

    // Constructor
    Task(int id, const string& name, const string& desc, const string& start, const string& end, const string& stat, int projID)
        : taskID(id), taskName(name), description(desc), startDate(start), endDate(end), status(stat), projectID(projID) {}

    virtual ~Task() {}  // Virtual destructor

    // Getter Functions
    int getTaskID() const { return taskID; }
    string getTaskName() const { return taskName; }
    string getDescription() const { return description; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    int getProjectID() const { return projectID; }

    // Getting status
    virtual string getStatus() const { return status; }
    // Getting duedate
    virtual string getDueDate() const { return endDate; }

    // Setter Functions
    void setTaskName(const string& name) { taskName = name; }
    void setDescription(const string& desc) { description = desc; }
    void setStartDate(const string& start) { startDate = start; }
    void setEndDate(const string& end) { endDate = end; }
    void setStatus(const string& stat) { status = stat; }
    void setProjectID(int id) { projectID = id; }

    // Type identifier for polymorphism
    virtual string getType() const { return "None"; }

    // For SubTasks, this returns the parent task ID. For others, return -1.
    virtual int getParentTaskID() const { return -1; }

    // Display task details
    virtual void display(const vector<Project*>& projects, const vector<Task*>& tasks) const {
        cout << "[" << getType() << "] Task ID: " << taskID << ", Name: " << taskName
             << ", Description: " << description << ", Start: " << startDate
             << ", End: " << endDate << ", Status: " << status << ", Project ID: " << projectID << endl;
    }
};

class MainTask : public Task {
public:
    using Task::Task;  // Inherit base constructors

    // Override type label
    string getType() const override { return "Main"; }

    // Display maintask details
    void display(const vector<Project*>& projects, const vector<Task*>& tasks) const override {
        string projectName = "Unknown";

        // Get project name from project ID
        for (const Project* p : projects) {
            if (p->getID() == projectID) {
                projectName = p->getProjectName();
                break;
            }
        }

        cout << "\n==========================================\n";
        cout << "[Main Task] ID: " << taskID << "\n";
        cout << "Name        : " << taskName << "\n";
        cout << "Description : " << description << "\n";
        cout << "Start Date  : " << startDate << "\n";
        cout << "End Date    : " << endDate << "\n";
        cout << "Status      : " << status << "\n";
        cout << "Project     : " << projectName << " [ID: " << projectID << "]\n";
        cout << "------------------------------------------\n";
    }
};

class SubTask : public Task {
protected:
    int parentTaskID;  // ID of the main task this sub task belongs to

public:
    // Default constructor
    SubTask() : parentTaskID(-1) {}

    // Constructor that includes parent task ID
    SubTask(int id, const string& name, const string& desc, const string& start, const string& end, const string& stat, int projID, int parentID)
        : Task(id, name, desc, start, end, stat, projID), parentTaskID(parentID) {}

    // Return the ID of the parent (main) task
    int getParentTaskID() const override { return parentTaskID; }

    // Allow setting a new parent ID (if changed)
    void setParentTaskID(int id) { parentTaskID = id; }

    // Label for identification
    string getType() const override { return "Sub"; }

    // display for sub-task details
    void display(const vector<Project*>& projects, const vector<Task*>& tasks) const override {
        string projectName = "Unknown";
        string parentName = "Unknown";

        // Get name of the project this task belongs to
        for (const Project* p : projects) {
            if (p->getID() == projectID) {
                projectName = p->getProjectName();
                break;
            }
        }

        // Get name of the parent (main) task by matching ID
        for (const Task* t : tasks) {
            if (t->getTaskID() == parentTaskID) {
                parentName = t->getTaskName();
                break;
            }
        }

        cout << "\n[Sub Task] ID: " << taskID << "\n";
        cout << "Name        : " << taskName << "\n";
        cout << "Description : " << description << "\n";
        cout << "Start Date  : " << startDate << "\n";
        cout << "End Date    : " << endDate << "\n";
        cout << "Status      : " << status << "\n";
        cout << "Project     : " << projectName << " [ID: " << projectID << "]\n";
        cout << "Parent Task : " << parentName << " [ID: " << parentTaskID << "]\n";
        cout << "------------------------------------------\n";
    }
};

#endif
