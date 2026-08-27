#include <iostream>
#include <cctype>
#include <sstream>
#include <limits>
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
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid number.\n";
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

        cout << "Invalid input. Please enter a number greater than 0.\n";
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

        cout << "Invalid input. Please enter 0 or a positive number.\n";
    }
}

// Read any double value from the user
// Returns the double value entered by the user
double getValidatedDouble(const string& prompt) {
    double value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid number.\n";
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

        cout << "Invalid input. Value cannot be negative.\n";
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

        cout << "Invalid input. This field cannot be empty.\n";
    }
}

// Read a Yes/No response from the user
// Accepts 'Y', 'y', 'N', 'n' and returns the uppercase character
char getYesNo(const string& prompt) {
    char answer;

    while (true) {
        cout << prompt;
        cin >> answer;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        answer = toupper(static_cast<unsigned char>(answer));

        if (answer == 'Y' || answer == 'N') {
            return answer;
        }

        cout << "Invalid input. Please enter Y or N.\n";
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

        cout << "Invalid choice. Please enter a number between "
             << minOption << " and " << maxOption << ".\n";
    }
}

// Pause execution and wait for the user to press 0 to continue
// Used after displaying results so the user can read before the menu reappears
void pressZeroToContinue() {
    int choice;

    while (true) {
        cout << "\nPress 0 to continue: ";

        if (cin >> choice) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 0) {
                break;
            }

            cout << "Invalid input. Please enter 0.\n";
        }
        else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter 0.\n";
        }
    }
}