#include <iostream>
#include <cstdlib>
#include "login_module.h"
#include "input_utils.h"
using namespace std;

// ============================================================
// STEP 1：validatePasswordLength() 
// ============================================================
bool validatePasswordLength(const string& password) {
    return (int)password.length() >= MIN_PASSWORD_LENGTH;
}

// ============================================================
// STEP 2：checkDuplicateUsername()
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
// STEP 3：findStaffIndex()
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
// STEP 4：login()
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
// STEP 5：registerUser() — 移除了 role 字段
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
            cout << "[ERROR] Password too short. Please try again.\n";
            continue;
        }
        break;
    }

    // 不再询问 role，直接存入空字符串
    staffList.push_back({ username, password, "" });
    cout << "\n[System] Account registered successfully! You can now log in.\n";
}

// ============================================================
// STEP 6：loginUser()
// ============================================================
bool loginUser() {
    string username, password;

    cout << "\n----- LOGIN -----\n";
    cout << "(Enter 0 to go back)\n";

    username = readNonEmptyLine("Enter username: ");

    if (username == "0") {
        cout << "[System] Login cancelled.\n";
        return false;
    }

    password = readNonEmptyLine("Enter password: ");

    int idx = findStaffIndex(username);
    if (idx != -1) {
        if (staffList[idx].password == password) {
            // 不再显示 role
            cout << "\n[System] Login successful! Welcome, " << username << ".\n";
            return true;
        }
        else {
            cout << "[ERROR] Incorrect password. Please try again.\n";
            return false;
        }
    }
    else {
        cout << "[ERROR] Username not found. Please register first.\n";
        return false;
    }
}

// ============================================================
// STEP 7：authenticationGate()
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
        }
    } while (!loggedIn);
}