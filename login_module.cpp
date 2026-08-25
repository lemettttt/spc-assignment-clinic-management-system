 
#include <iostream>
#include <cstdlib>
#include "login_module.h"
#include "input_utils.h"
using namespace std;

// ============================================================
// STEP 1：validatePasswordLength() 
// ============================================================
bool validatePasswordLength(const string& password) {
	return (int)password.length() >= MIN_PASSWORD_LENGTH;   // 6 characters minimum
}

// ============================================================
// STEP 2：checkDuplicateUsername()
// ============================================================
bool checkDuplicateUsername(const string& username) {
	// Check if the username already exists in the staffList
    for (int i = 0; i < (int)staffList.size(); i++) {

		// Check if the username matches any existing staff member's username
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
	// Search for the index of the staff member with the given username in the staffList
    for (int i = 0; i < (int)staffList.size(); i++) {

		// Check if the username matches any existing staff member's username
        if (staffList[i].username == username) {
            return i;
        }
    }
    return -1;
}

// ============================================================
// STEP 4：login()
// for loop(linear search)整个staffList，username & password 都对上才回传true
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
// STEP 5：registerUser()
// ============================================================
void registerUser() {
    string username, password, role;

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

    role = readAlphaOnly("Enter role (e.g. Nurse / Doctor / Pharmacist ): ");

    staffList.push_back({ username, password, role });
    cout << "\n[System] Account registered successfully! You can now log in.\n";
}

// ============================================================
// STEP 6：loginUser()
// nested if —— 先判断username存不存在，存在的话再判断password对不对
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
            cout << "\n[System] Login successful! Welcome, " << username
                << " (" << staffList[idx].role << ").\n";
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
// STEP 7：authenticationGate() —— used in main.cpp
// ------------------------------------------------------------
void authenticationGate() {
    int choice;
    bool loggedIn = false;

	// Use a do-while loop to keep prompting the user until they successfully log in or choose to exit
    do {
        cout << "\n===== TARUMT CLINIC - STAFF LOGIN =====\n";
        cout << "1. Login\n";
        cout << "2. Register New Account\n";
        cout << "3. Exit System\n";
        cout << "=========================================\n";
        choice = readValidatedInt("Enter your choice: ", 1, 3);

        switch (choice) {
        case 1:
			loggedIn = loginUser();     // If login is successful, loggedIn will be true and the loop will exit
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
	} while (!loggedIn);    // Keep looping until the user successfully logs in
}