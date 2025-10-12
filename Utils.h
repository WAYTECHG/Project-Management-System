#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <tuple>
#include <sstream>
#include <stdexcept>
#include <cstdio>

using namespace std;

class Utils {
public:

    // Repeatedly prompts until a valid integer is entered.
    static int getValidInt(const string& prompt) {
        string input;
        int value;

        while (true) {
            cout << prompt;
            getline(cin, input);
            input = whitespace(input);  // remove surrounding spaces

            try {
                if (input.empty())
                    throw invalid_argument("Input cannot be empty. Please enter a number.");

                value = stoi(input);  // convert string to integer; throws error if invalid

                return value;

            } catch (const invalid_argument& e) {
                cerr << "Invalid input: " << e.what() << "\n";
            } catch (const out_of_range& e) {
                cerr << "Number too large or too small. Try again.\n";
            }
        }
    }

    // Removes leading and trailing whitespace from the string
    static string whitespace(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n"); // find first non-space character
        size_t last = str.find_last_not_of(" \t\r\n");   // find last non-space character
        if (first == string::npos || last == string::npos)
            return "";  // string is all whitespace
        return str.substr(first, last - first + 1);  // extract trimmed substring
    }

    // Removes surrounding double quotes
    static string trim(const string& str) {
        string s = str;
        if (!s.empty() && s.front() == '"') s.erase(0, 1);  // remove first character if it's a quote
        if (!s.empty() && s.back() == '"') s.pop_back();    // remove last character if it's a quote
        return s;
    }

    // Converts all characters in a string to lowercase
    static string toLower(const string& str) {
        string lowered = str;
        transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
        return lowered;
    }

    // Keeps asking user for input until they type something non-empty
    static string getNonEmptyInput(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = whitespace(input);
            if (input.empty()) {
                cout << "Input cannot be empty. Please try again.\n";
            } else {
                return input;
            }
        }
    }

    // Asks the user to choose an item from a numbered list (starting from 1),
    // but converts the chosen number to match how computers count (starting from 0).
    static int getValidIndexSelection(int listSize, const string& label) {
        int index = -1;
        string input;

        while (true) {
            cout << "Enter the number of the " << label << ": ";
            getline(cin, input);
            input = whitespace(input);

            try {
                if (input.empty())
                    throw invalid_argument("Input cannot be empty.");

                if (!all_of(input.begin(), input.end(), ::isdigit))  // checks if all chars are digits
                    throw invalid_argument("Input must be numeric.");

                index = stoi(input);

                if (index < 1 || index > listSize)
                    throw out_of_range("Selection out of range.");

                return index - 1; // User inputs 1 for first item, but program uses index 0

            } catch (const exception& e) {
                cerr << "Error: " << e.what() << "\nPlease try again.\n";
            }
        }
    }

    // Returns today's date in format dd/mm/yyyy
    static string getTodayDate() {
        time_t now = time(nullptr);
        tm* local = localtime(&now);  // convert to local time

        char buffer[11];  // enough space for "dd/mm/yyyy"
        snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);

        return string(buffer);
    }

    // Compares two dates (strings) and returns:
    // -1 if d1 < d2, 1 if d1 > d2, 0 if equal
    static int compareDates(const string& d1, const string& d2) {
        auto parse = [](const string& date) -> tuple<int, int, int> {
            int day, month, year;
            char sep;
            istringstream iss(date);

            // Parse the format dd/mm/yyyy
            if (!(iss >> day >> sep >> month >> sep >> year) || sep != '/' || day < 1 || month < 1 || month > 12 || year < 1000) {
                throw invalid_argument("Invalid date format: " + date);
            }
            return make_tuple(year, month, day);  // tuple for comparison
        };

        try {
            auto t1 = parse(d1);
            auto t2 = parse(d2);

            if (t1 < t2) return -1;
            if (t1 > t2) return 1;
            return 0;
        } catch (const std::invalid_argument& e) {
            cout << "Error: " << e.what() << endl;
            return 0;  // if parsing fails, treat as equal (neutral result)
        }
    }

    // Checks if a task's date range is inside the project's date range
    static bool isDateWithinProjectRange(const string& taskStart, const string& taskEnd, const string& projectStart, const string& projectEnd) {
        return compareDates(projectStart, taskStart) <= 0 &&
               compareDates(taskEnd, projectEnd) <= 0 &&
               compareDates(taskStart, taskEnd) <= 0;
    }

    // Same idea, but used for subtasks inside main tasks
    static bool isDateWithinRange(const string& childStart, const string& childEnd, const string& parentStart, const string& parentEnd) {
        return compareDates(parentStart, childStart) <= 0 &&
               compareDates(childEnd, parentEnd) <= 0 &&
               compareDates(childStart, childEnd) <= 0;
    }

    // Splits a line of CSV into individual values, accounting for quotes
    static vector<string> parseCSVRow(const string& line) {
        vector<string> result;
        bool inQuotes = false;
        string value;

        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;  // toggle quote status
            } else if (c == ',' && !inQuotes) {
                result.push_back(trim(value));  // end of a value
                value.clear();
            } else {
                value += c;
            }
        }

        result.push_back(trim(value));  // add last field
        return result;
    }

    // Ensures a number entered by user is within given range
    static int getValidatedNumber(const string& prompt, int min, int max) {
        string input;
        int number;

        while (true) {
            cout << prompt;
            getline(cin, input);
            input = whitespace(input);

            if (input.empty()) {
                cout << "Input cannot be empty.\n";
                continue;
            }

            if (!all_of(input.begin(), input.end(), ::isdigit)) {
                cout << "Invalid input. Please enter a numeric value.\n";
                continue;
            }

            try {
                number = stoi(input);
                if (number < min || number > max) {
                    cout << "Value must be between " << min << " and " << max << ".\n";
                    continue;
                }
                return number;
            } catch (...) {
                cout << "Error converting input to number. Try again.\n";
            }
        }
    }

    // Guides user through entering a valid date in parts: day, month, year
    static string getValidDate(const string& label) {
        while (true) {
            try {
                cout << label << " (Format: dd/mm/yyyy):\n";

                int day = getValidatedNumber("Enter day (1-31): ", 1, 31);
                int month = getValidatedNumber("Enter month (1-12): ", 1, 12);
                int year = getValidatedNumber("Enter year (>= 1900): ", 1900, 9999);

                // Check leap year for February
                bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
                int maxDayInMonth[] = {31, (isLeap ? 29 : 28), 31, 30, 31, 30,
                                       31, 31, 30, 31, 30, 31};

                if (day > maxDayInMonth[month - 1]) {
                    cout << "Invalid date. That month only has " << maxDayInMonth[month - 1] << " days.\n";
                    continue;
                }

                // Format as "dd/mm/yyyy"
                string dateFormat = (day < 10 ? "0" : "") + to_string(day) + "/" +
                                    (month < 10 ? "0" : "") + to_string(month) + "/" +
                                    to_string(year);

                cout << "Date entered: " << dateFormat << "\n\n";
                return dateFormat;

            } catch (const exception& e) {
                cerr << "Unexpected error: " << e.what() << "\nPlease re-enter the date.\n\n";
            }
        }
    }

    // Compares two dates to make sure the start is not after the end
    static bool isValidDateRange(const string& start, const string& end) {
        auto parseDate = [](const string& date) -> tuple<int, int, int> {
            istringstream iss(date);
            string token;
            int d = 0, m = 0, y = 0;

            try {
                // Split by '/' and convert to integers
                if (!getline(iss, token, '/')) throw invalid_argument("Invalid date format (missing day)");
                d = stoi(token);

                if (!getline(iss, token, '/')) throw invalid_argument("Invalid date format (missing month)");
                m = stoi(token);

                if (!getline(iss, token)) throw invalid_argument("Invalid date format (missing year)");
                y = stoi(token);

                if (d < 1 || d > 31 || m < 1 || m > 12 || y < 1900)
                    throw out_of_range("Date components out of valid range.");

            } catch (const exception& e) {
                cerr << "Error parsing date \"" << date << "\": " << e.what() << endl;
                return {0, 0, 0}; // marks invalid date
            }

            return {y, m, d}; // return tuple in order of year, month, day
        };

        return parseDate(start) <= parseDate(end);
    }

    // Menu for choosing task status
    static string getStatusUser() {
        string input;
        int choice = 0;

        while (true) {
            try {
                cout << "Select status:\n1. Not Started\n2. In Progress\n3. Completed\nEnter Choice: ";
                getline(cin, input);

                if (input.empty())
                    throw invalid_argument("Input cannot be empty.");

                choice = stoi(input);

                if (choice < 1 || choice > 3)
                    throw out_of_range("Choice must be between 1 and 3.");

                return (choice == 1 ? "Not Started" : (choice == 2 ? "In Progress" : "Completed"));

            } catch (const exception& e) {
                cerr << "Error: " << e.what() << "\n";
            }
        }
    }

    // Validates phone number format: must be digits, optionally starting with '+'
    static string getValidPhoneNumber(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = whitespace(input);  // Remove any whitespace from input

            if (input.empty()) {
                cout << "Phone number cannot be empty.\n";
                continue;
            }

            // Handle optional '+' prefix by skipping it in digit validation
            size_t startIdx = (input[0] == '+') ? 1 : 0;

            // Validate all remaining characters are digits
            bool valid = all_of(input.begin() + startIdx, input.end(), ::isdigit);
            if (!valid) {
                cout << "Phone number must contain only digits (optionally starting with '+').\n";
                continue;
            }

            // Validate length (7-15 chars) including optional '+'
            if (input.length() < 7 || input.length() > 15) {
                cout << "Phone number must be between 7 and 15 characters long.\n";
                continue;
            }

            return input;
        }
    }

    // Capitalizes first letter, lowers the rest (e.g., "hello" → "Hello")
    static string capitalize(const string& input) {
        if (input.empty()) return "";

        string result = input;
        result[0] = toupper(result[0]);  // make first letter uppercase
        for (size_t i = 1; i < result.size(); ++i) {
            result[i] = tolower(result[i]);  // make rest lowercase
        }

        return result;
    }

};

#endif
