#include <iostream>
#include <cctype>
#include <sstream>
#include <limits>
#include <ctime>
#include "input_utils.h"
using namespace std;

// ============================================================
// Validated Integer Input (range)
// ============================================================

int readValidatedInt(const string& prompt, int minVal, int maxVal) {
    int value;
    string line;

    do {
        cout << prompt;
        getline(cin, line);

        if (line.empty()) {
            cout << "[ERROR] This field cannot be empty. Please try again.\n";
            continue;
        }

        stringstream ss(line);

        if ((ss >> value) && ss.eof()) {
            if (value < minVal || value > maxVal) {
                cout << "[ERROR] Please enter a number between " << minVal << " and " << maxVal << ".\n";
            }
            else {
                return value;
            }
        }
        else {
            cout << "[ERROR] Invalid input. Please enter numbers only.\n";
        }

    } while (true);
}

// ============================================================
// Validated Gender (M/F)
// ============================================================

char readValidatedGender() {
    string input;
    char gender;

    do {
        cout << "\nEnter Gender (M/F): ";
        getline(cin, input);

        if (input.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter M or F.\n";
            continue;
        }

        if (input.length() == 1) {
            gender = toupper(input[0]);
            if (gender == 'M' || gender == 'F') {
                return gender;
            }
        }

        cout << "[ERROR] Invalid gender. Please enter M or F only.\n";

    } while (true);
}

// ============================================================
// Read Non‑Empty Line
// ============================================================

string readNonEmptyLine(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (!input.empty()) {
            return input;
        }
        cout << "[ERROR] This field cannot be empty. Please try again.\n";
    }
}

// ============================================================
// Validated Double (with minimum value)
// ============================================================

double readValidatedDouble(const string& prompt, double minVal) {
    string line;
    double value;
    bool valid = false;

    do {
        cout << prompt;
        getline(cin, line);

        if (line.empty()) {
            cout << "[ERROR] This field cannot be empty. Please try again.\n";
            continue;
        }

        stringstream ss(line);

        if ((ss >> value) && ss.eof()) {
            if (value >= minVal) {
                valid = true;
            }
            else {
                cout << "[ERROR] Value must be at least " << minVal << ".\n";
            }
        }
        else {
            cout << "[ERROR] Invalid input. Please enter numbers only.\n";
        }

    } while (!valid);

    return value;
}

// ============================================================
// Validated Phone Number (digits and hyphen)
// ============================================================

string readValidatedPhone(const string& prompt) {
    string input;
    bool valid = false;

    do {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "[ERROR] This field cannot be empty. Please try again.\n";
            continue;
        }

        int digitCount = 0;
        valid = true;

        for (int i = 0; i < input.length(); i++) {
            char c = input[i];
            if (isdigit(c)) {
                digitCount++;
            }
            else if (c != '-') {
                valid = false;
                break;
            }
        }

        if (!valid) {
            cout << "[ERROR] Phone number can only contain digits and '-'.\n";
            continue;
        }

        if (digitCount < 9 || digitCount > 11) {
            cout << "[ERROR] Phone number must contain between 9 and 11 digits.\n";
            valid = false;
        }

    } while (!valid);

    return input;
}

// ============================================================
// Read Alphabetic Only (letters and spaces)
// ============================================================

string readAlphaOnly(const string& prompt) {
    string input;
    bool valid = false;

    do {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "[ERROR] This field cannot be empty. Please try again.\n";
            continue;
        }

        valid = true;
        for (int i = 0; i < input.length(); i++) {
            char c = input[i];
            if (!isalpha(c) && c != ' ') {
                valid = false;
                break;
            }
        }

        if (!valid) {
            cout << "[ERROR] Only alphabet letters are allowed. Please try again.\n";
        }

    } while (!valid);

    return input;
}

// ============================================================
// Generic Validated Integer (no range)
// ============================================================

int getValidatedInt(const string& prompt) {
    string line;
    int value;

    while (true) {
        cout << prompt;
        getline(cin, line);

        if (line.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter a valid number.\n";
            continue;
        }

        stringstream ss(line);

        if ((ss >> value) && ss.eof()) {
            return value;
        }

        cout << "[ERROR] Invalid input. Please enter a valid number.\n";
    }
}

// ============================================================
// Read Positive Integer (>0)
// ============================================================

int getPositiveInt(const string& prompt) {
    int value;
    while (true) {
        value = getValidatedInt(prompt);
        if (value > 0) {
            return value;
        }
        cout << "[ERROR] Please enter a number greater than 0.\n";
    }
}

// ============================================================
// Read Non‑Negative Integer (>=0)
// ============================================================

int getNonNegativeInt(const string& prompt) {
    int value;
    while (true) {
        value = getValidatedInt(prompt);
        if (value >= 0) {
            return value;
        }
        cout << "[ERROR] Please enter 0 or a positive number.\n";
    }
}

// ============================================================
// Generic Validated Double
// ============================================================

double getValidatedDouble(const string& prompt) {
    string line;
    double value;

    while (true) {
        cout << prompt;
        getline(cin, line);

        if (line.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter a valid number.\n";
            continue;
        }

        stringstream ss(line);

        if ((ss >> value) && ss.eof()) {
            return value;
        }

        cout << "[ERROR] Invalid input. Please enter a valid number.\n";
    }
}

// ============================================================
// Read Non‑Negative Double (>=0)
// ============================================================

double getNonNegativeDouble(const string& prompt) {
    double value;
    while (true) {
        value = getValidatedDouble(prompt);
        if (value >= 0) {
            return value;
        }
        cout << "[ERROR] Value cannot be negative.\n";
    }
}

// ============================================================
// Read Non‑Empty String
// ============================================================

string getNonEmptyString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (!value.empty()) {
            return value;
        }
        cout << "[ERROR] Invalid input. This field cannot be empty.\n";
    }
}

// ============================================================
// Read Yes/No (Y/N)
// ============================================================

char getYesNo(const string& prompt) {
    string input;
    char answer;

    while (true) {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter Y or N.\n";
            continue;
        }

        if (input.length() != 1) {
            cout << "[ERROR] Please enter only Y or N.\n";
            continue;
        }

        answer = toupper(static_cast<unsigned char>(input[0]));

        if (answer == 'Y' || answer == 'N') {
            return answer;
        }

        cout << "[ERROR] Invalid input. Please enter Y or N.\n";
    }
}

// ============================================================
// Read Menu Choice (within range)
// ============================================================

int getValidatedMenuChoice(const string& prompt, int minOption, int maxOption) {
    int choice;
    while (true) {
        choice = getValidatedInt(prompt);
        if (choice >= minOption && choice <= maxOption) {
            return choice;
        }
        cout << "[ERROR] Please enter a number between "
             << minOption << " and " << maxOption << ".\n";
    }
}

// ============================================================
// Wait for 0 to Continue
// ============================================================

void pressZeroToExit() {
    string line;
    int choice;

    while (true) {
        cout << "\nPress 0 to exit: ";
        getline(cin, line);

        if (line.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter 0.\n";
            continue;
        }

        stringstream ss(line);

        if ((ss >> choice) && ss.eof()) {
            if (choice == 0) {
                break;
            }
            cout << "[ERROR] Invalid input. Please enter 0.\n";
        } else {
            cout << "[ERROR] Invalid input. Please enter a valid number.\n";
        }
    }
}

// ============================================================
// Pause Until User Presses Enter 
// ============================================================

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

// ============================================================
// Clear Screen
// ============================================================

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ============================================================
// Date Validation (any valid date, past/present/future)
// ============================================================

bool isValidDate(const string& date)
{
    if (date.length() != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;

    for (int i = 0; i < (int)date.length(); i++) {
        if (i != 2 && i != 5) {
            if (!isdigit((unsigned char)date[i])) return false;
        }
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (month < 1 || month > 12) return false;

    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1]) return false;
    if (year < 2010 || year > 2030) return false;

    return true;
}

// ============================================================
// Prompt for Valid Date (any valid date)
// ============================================================

string getValidatedDate(const string& prompt) {
    string date;
    while (true) {
        cout << prompt;
        getline(cin, date);
        if (date.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter a date in DD/MM/YYYY format.\n";
            continue;
        }
        if (!isValidDate(date)) {
            cout << "[ERROR] Invalid date format. Please use DD/MM/YYYY (e.g., 25/12/2026).\n";
            continue;
        }
        return date;
    }
}

// ============================================================
// Future Date Validation (today or later)
// ============================================================

bool isValidFutureDate(const string& date)
{
    if (!isValidDate(date)) return false;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int todayYear  = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay   = localTime->tm_mday;

    if (year < todayYear ||
        (year == todayYear && month < todayMonth) ||
        (year == todayYear && month == todayMonth && day < todayDay)) {
        return false;
    }
    return true;
}

// ============================================================
// Prompt for Future Date (today or later)
// ============================================================

string getValidatedFutureDate(const string& prompt)
{
    string date;
    while (true) {
        cout << prompt;
        getline(cin, date);
        if (date.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter today's date or a future date in DD/MM/YYYY format.\n";
            continue;
        }
        if (!isValidFutureDate(date)) {
            cout << "[ERROR] Invalid date. Please enter today's date or a future date in DD/MM/YYYY format.\n";
            continue;
        }
        return date;
    }
}

// ============================================================
// Expiry Date Validation (strictly after today)
// ============================================================

bool isValidExpiryDate(const string& date)
{
    if (!isValidDate(date)) return false;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int todayYear  = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay   = localTime->tm_mday;

    if (year < todayYear ||
        (year == todayYear && month < todayMonth) ||
        (year == todayYear && month == todayMonth && day <= todayDay)) {
        return false;
    }
    return true;
}

// ============================================================
// Prompt for Expiry Date (must be after today)
// ============================================================

string getValidatedExpiryDate(const string& prompt) {
    string date;
    while (true) {
        cout << prompt;
        getline(cin, date);
        if (date.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter a future date in DD/MM/YYYY format.\n";
            continue;
        }
        if (!isValidExpiryDate(date)) {
            cout << "[ERROR] Invalid expiry date. Please enter a future date in DD/MM/YYYY format (must be after today).\n";
            continue;
        }
        return date;
    }
}