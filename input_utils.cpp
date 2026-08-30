#include <iostream>
#include <cctype>
#include <sstream>
#include <limits>
#include <ctime>
#include "input_utils.h"
using namespace std;

// Read an integer until the user enters a valid number (range can be specified)
// Eg：int choice = readValidatedInt("Enter your choice: ", 1, 6);
int readValidatedInt(const string& prompt, int minVal, int maxVal) {
    int value;
    string line;

    // Use a do-while loop to repeatedly prompt the user until valid input is received
    do {
        cout << prompt;
        getline(cin, line);

        if (line.empty()) {
            cout << "[ERROR] This field cannot be empty. Please try again.\n";
            continue;
        }

        stringstream ss(line);  // Use stringstream to check if the input is a valid integer

        // Check if the input is a valid integer and within the specified range
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

// Only accepts one character: M/m/F/f. Other inputs are invalid.
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

// Used for required fields such as name and phone number.
// The user cannot leave the field empty.
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

// Used for decimal values such as price or discount.
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

        stringstream ss(line);  // Use stringstream to check if the input is a valid double

        // Check if the input is a valid double and greater than or equal to minVal
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

// Used for phone numbers. Allows digits and '-'.
string readValidatedPhone(const string& prompt) {
    string input;
    bool valid = false;     // Flag to track if the input is valid

    // Use a do-while loop to repeatedly prompt the user until valid input is received
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

// Only alphabet letters and spaces are allowed.
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

// Read any integer from the user (no range validation)
// Returns the integer value entered by the user
int getValidatedInt(const string& prompt) {
    string line;
    int value;

    while (true) {
        cout << prompt;
        getline(cin, line);
        
        // Check for empty input
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

// Read a positive integer (greater than 0)
// Prompts repeatedly until the user enters a valid positive number
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

// Read a non-negative integer (0 or greater)
// Prompts repeatedly until the user enters a valid non-negative number
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

// Read any double value from the user
// Returns the double value entered by the user
double getValidatedDouble(const string& prompt) {
    string line;
    double value;

    while (true) {
        cout << prompt;
        getline(cin, line);
        
        // Check for empty input
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

// Read a non-negative double (0.0 or greater)
// Prompts repeatedly until the user enters a valid non-negative number
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

// Read a non-empty string from the user
// Rejects empty input and prompts until the user enters something
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

// Read a Yes/No response from the user
// Accepts 'Y', 'y', 'N', 'n' and returns the uppercase character
char getYesNo(const string& prompt) {
    string input;
    char answer;

    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Check for empty input
        if (input.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter Y or N.\n";
            continue;
        }
        
        // Check if input is exactly one character
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

// Read a menu choice within a specified range
// Prompts until the user enters a valid option between minOption and maxOption
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

// Pause execution and wait for the user to press 0 to continue
// Used after displaying results so the user can read before the menu reappears
void pressZeroToContinue() {
    string line;
    int choice;

    while (true) {
        cout << "\nPress 0 to continue: ";
        getline(cin, line);
        
        // Check for empty input
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

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// ============================================================
// 1. BASIC DATE VALIDATION - Format only (DD/MM/YYYY)
//    Allows ANY valid date (past, present, or future)
//    Used for: Consultation dates, medical history, general records
// ============================================================

bool isValidDate(const string& date)
{
    // Check length: DD/MM/YYYY = 10 characters
    if (date.length() != 10)
    {
        return false;
    }

    // Check slash positions
    if (date[2] != '/' || date[5] != '/')
    {
        return false;
    }

    // Check all other characters are digits
    for (int i = 0; i < (int)date.length(); i++)
    {
        if (i != 2 && i != 5)
        {
            if (!isdigit((unsigned char)date[i]))
            {
                return false;
            }
        }
    }

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (month < 1 || month > 12)
    {
        return false;
    }

    int daysInMonth[12] =
    {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    // Leap year
    if ((year % 400 == 0) ||
        (year % 4 == 0 && year % 100 != 0))
    {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
    {
        return false;
    }

    // Allow reasonable range (2010-2030)
    if (year < 2010 || year > 2030)
    {
        return false;
    }

    return true;
}

// ============================================================
// PROMPT + VALIDATE A DATE (Format only)
//    Allows ANY valid date (past, present, or future)
//    Used for: Consultation dates, medical history
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
// 2. FUTURE DATE VALIDATION - Must be today or future
//    Does NOT allow past dates
//    Used for: Appointments, bookings, scheduling
// ============================================================

bool isValidFutureDate(const string& date)
{
    // First check basic format
    if (!isValidDate(date))
    {
        return false;
    }

    // Parse the date
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    // Get today's date
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);

    int todayYear  = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay   = localTime->tm_mday;

    // Date must be today or in the future
    if (year < todayYear ||
        (year == todayYear && month < todayMonth) ||
        (year == todayYear && month == todayMonth && day < todayDay))
    {
        return false;
    }

    return true;
}

// ============================================================
// PROMPT + VALIDATE FUTURE DATE (Today or future)
//    Used for: Appointments, bookings
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
// 3. EXPIRY DATE VALIDATION - Must be in the future (after today)
//    Used for: Medicine expiry dates
// ============================================================

bool isValidExpiryDate(const string& date)
{
    // First check basic format
    if (!isValidDate(date))
    {
        return false;
    }

    // Parse the date
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    // Get today's date
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);

    int todayYear  = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay   = localTime->tm_mday;

    // Expiry date must be strictly in the future (cannot be today)
    if (year < todayYear ||
        (year == todayYear && month < todayMonth) ||
        (year == todayYear && month == todayMonth && day <= todayDay))
    {
        return false;
    }

    return true;
}

// ============================================================
// PROMPT + VALIDATE EXPIRY DATE (Must be after today)
//    Used for: Medicines
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