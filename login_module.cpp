#include <iostream>
#include <cstdlib>
#include "login_module.h"
#include "input_utils.h"
using namespace std;

// ============================================================
// Check Password Length (minimum characters)
// ============================================================

bool validatePasswordLength(const string& password) {
    return (int)password.length() >= MIN_PASSWORD_LENGTH;
}

// ============================================================
// Check for Duplicate Username
// ============================================================

bool checkDuplicateUsername(const string& username) {
    for (int i = 0; i < (int)staffList.size(); i++) {
        if (staffList[i].username == username) {
            return true;
        }
    }
    return false;
}

// ============================================================
// Find Staff Index by Username
// ============================================================

int findStaffIndex(const string& username) {
    for (int i = 0; i < (int)staffList.size(); i++) {
        if (staffList[i].username == username) {
            return i;
        }
    }
    return -1;
}

// ============================================================
// Authenticate User (username + password)
// ============================================================

bool login(const vector<Staff>& staffList, const string& username, const string& password) {
    for (int i = 0; i < (int)staffList.size(); i++) {
        if (staffList[i].username == username && staffList[i].password == password) {
            return true;
        }
    }
    return false;
}

// ============================================================
// Register New User (role field omitted)
// ============================================================

void registerUser() {
    string username, password;

    cout << "\n----- REGISTER NEW STAFF ACCOUNT -----\n";
    cout << "(Enter 0 to go back)\n";

    while (true) {
        username = readNonEmptyLine("Enter new username: ");
        if (username == "0") {
            cout << "[System] Registration cancelled.\n";
            return;
        }
        if (checkDuplicateUsername(username)) {
            cout << "[ERROR] This username is already taken. Please choose another one.\n";
            continue;
        }
        break;
    }

    while (true) {
        password = readNonEmptyLine("Enter new password (at least " + to_string(MIN_PASSWORD_LENGTH) + " characters): ");

        if (password == "0") {
            cout << "[System] Registration cancelled.\n";
            return;
        }

        if (!validatePasswordLength(password)) {
            cout << "[ERROR] Password too short. Please try again.\n\n";
            continue;
        }
        break;
    }

    staffList.push_back({ username, password, "" });
    cout << "\n[System] Account registered successfully! You can now log in.\n";
    waitForEnter();
    clearScreen();
}

// ============================================================
// User Login Flow
// ============================================================

bool loginUser() {
    string username, password;
    const int MAX_ATTEMPTS = 3;

    // ----- Username Attempts (3 times) -----
    int userAttempts = 0;
    int idx = -1;

    while (userAttempts < MAX_ATTEMPTS) {
        cout << "\n----- LOGIN -----";
        cout << "\n(Enter 0 to go back)\n";
        username = readNonEmptyLine("Enter Username (" + to_string(userAttempts + 1) + "/" + to_string(MAX_ATTEMPTS) + "): ");

        if (username == "0") {
            cout << "[System] Login cancelled.\n";
            return false;
        }

        idx = findStaffIndex(username);

        if (idx != -1) {
            break;
        } else {
            userAttempts++;
            cout << "[ERROR] Username not found. Please register first.\n";
            if (userAttempts >= MAX_ATTEMPTS) {
                cout << "[System] Maximum username attempts reached.\n";
                return false;
            }
        }
    }

    // ----- Password Attempts (3 times) -----
    int passAttempts = 0;

    while (passAttempts < MAX_ATTEMPTS) {
        cout << "\n----- PASSWORD -----\n";
        cout << "Username: " << username << "\n";
        password = readNonEmptyLine("Enter Password (" + to_string(passAttempts + 1) + "/" + to_string(MAX_ATTEMPTS) + "): ");

        if (staffList[idx].password == password) {
            cout << "\n[System] Login successful! Welcome, " << username << ".\n";
            return true;
        } else {
            passAttempts++;
            cout << "[ERROR] Incorrect password. Please try again.\n";
            if (passAttempts >= MAX_ATTEMPTS) {
                cout << "[System] Maximum password attempts reached.\n";
                return false;
            }
        }
    }

    return false;
}

// ============================================================
// Authentication Gate (Login/Register/Exit loop)
// ============================================================

void authenticationGate() {
    int choice;
    bool loggedIn = false;

    do {
        setColor(COLOR_DARK_BLUE);
        cout << "\n  ============================================================\n";
        cout << "  |              TARUMT CLINIC - STAFF LOGIN                 |\n";
        cout << "  +----------------------------------------------------------+\n";
        cout << "  |                                                          |\n";

        setColor(COLOR_LIGHT_BLUE);
        cout << "  |   1.  Login                                              |\n";
        cout << "  |   2.  Register New Account                               |\n";

        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";

        setColor(COLOR_RED);
        cout << "  |   3.  Exit System                                        |\n";

        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        cout << "  ============================================================\n";
        setColor(COLOR_RESET);

        choice = readValidatedInt("  Enter your choice: ", 1, 3);

        switch (choice) {
        case 1:
            loggedIn = loginUser();
            break;
        case 2:
            registerUser();
            break;
        case 3:
            cout << "\nGoodbye!\n";
            exit(0);
        default:
            cout << "[ERROR] Invalid choice. Please enter 1-3.\n";
            waitForEnter();
        }
    } while (!loggedIn);

    // Login successful: press Enter to continue, then clear screen
    waitForEnter();
    clearScreen();
}