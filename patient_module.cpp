/*
    ============================================================
    Patient Management Module
    Chia Koh Suan
    ============================================================
 */

#include <iostream>
#include <iomanip>
#include <cctype>
#include "patient_module.h"
#include "input_utils.h"
using namespace std;

// Validate IC format (XXXXXX-XX-XXXX)
bool validateICFormat(const string& ic) {
    if (ic.length() != IC_LENGTH) {
        return false;
    }

    for (int i = 0; i < IC_LENGTH; i++) {
        if (i == 6 || i == 9) {
            if (ic[i] != '-') {
                return false;
            }
        }
        else {
            if (!isdigit(ic[i])) {
                return false;
            }
        }
    }

    return true;
}

bool equalsIgnoreCase(const string& a, const string& b) {
    if (a.length() != b.length()) return false;
    
    for (size_t i = 0; i < a.length(); i++) {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) {
            return false;
        }
    }
    return true;
}

// Check for duplicate IC number
bool checkDuplicateIC(const string& ic) {
    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].icNumber == ic)
            return true;
    }
    return false;
}

// Search patient by IC; returns index or -1
int searchPatientByIC(const string& ic) {
    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].icNumber == ic)
            return i;
    }
    return -1;
}

// Search patient by name; returns index or -1
int searchPatientByName(const string& name) {
    for (int i = 0; i < (int)patients.size(); i++) {
        if (equalsIgnoreCase(patients[i].name, name))
            return i;
    }
    return -1;
}

// Display a single patient record
void displayPatientRecord(const Patient& p) {
    cout << "\n---------------------------------------------\n";
    cout << left << setw(16) << "Patient ID"  << ": " << p.patientID << "\n";
    cout << left << setw(16) << "IC Number"   << ": " << p.icNumber << "\n";
    cout << left << setw(16) << "Name"        << ": " << p.name << "\n";
    cout << left << setw(16) << "Age"         << ": " << p.age << "\n";
    cout << left << setw(16) << "Gender"      << ": " << p.gender << "\n";
    cout << left << setw(16) << "Phone"       << ": " << p.phone << "\n";
    cout << left << setw(16) << "Allergies"   << ": ";

    if (p.allergyHistory.empty()) {
        cout << "None";
    }
    else {
        cout << p.allergyHistory;
    }

    cout << "\n";
    cout << "---------------------------------------------\n";
}

// Display all patient records in a table
void viewAllPatients() {
    cout << "\n============================= ALL PATIENTS =============================\n";

    if (patients.empty()) {
        cout << "  (No patient records found)\n";
        pressZeroToExit();
        return;
    }

    cout << left
        << setw(6) << "ID"
        << setw(16) << "IC Number"
        << setw(18) << "Name"
        << setw(5) << "Age"
        << setw(5) << "Gen"
        << setw(14) << "Phone"
        << "Allergy\n";

    cout << string(80, '-') << "\n";

    for (int i = 0; i < (int)patients.size(); i++) {
        const Patient& p = patients[i];
        cout << left
            << setw(6) << p.patientID
            << setw(16) << p.icNumber
            << setw(18) << p.name
            << setw(5) << p.age
            << setw(5) << p.gender
            << setw(14) << p.phone;

        if (p.allergyHistory.empty()) {
            cout << "None";
        }
        else {
            cout << p.allergyHistory;
        }

        cout << "\n";
    }

    cout << "==========================================================================\n";
    waitForEnter();
}

// Register a new patient
void registerNewPatient() {
    string ic, name, phone, allergy;
    int age;
    char gender;

    cout << "\n----- REGISTER NEW PATIENT -----\n";

    while (true) {
        cout << "\nEnter IC Number (format: XXXXXX-XX-XXXX, or 0 to cancel): ";
        getline(cin, ic);

        if (ic == "0") {
            cout << "[System] Registration cancelled.\n";
            waitForEnter();
            return;
        }

        if (!validateICFormat(ic)) {
            cout << "[ERROR] Invalid IC format. Please use format XXXXXX-XX-XXXX.\n";
            continue;
        }
        break;
    }

    if (checkDuplicateIC(ic)) {
        int idx = searchPatientByIC(ic);
        cout << "[ERROR] Duplicate IC. Already registered as Patient ID " << patients[idx].patientID << " (" << patients[idx].name << ").\n";
        cout << "[System] Registration cancelled.\n";
        waitForEnter();
        return;
    }

    name = readNonEmptyLine("\nEnter Full Name: ");
    age = readValidatedInt("\nEnter Age: ", 0, 150);
    gender = readValidatedGender();
    phone = readValidatedPhone("\nEnter Phone Number (e.g. 012-3456789): ");

    cout << "\nEnter Allergy History (or type None): ";
    getline(cin, allergy);
    if (allergy == "None" || allergy == "none") {
        allergy = "";
    }

    Patient p;
    p.patientID = nextPatientID++;
    p.icNumber = ic;
    p.name = name;
    p.age = age;
    p.gender = gender;
    p.phone = phone;
    p.allergyHistory = allergy;
    patients.push_back(p);

    cout << "\n[System] Patient registered successfully! Patient ID: " << p.patientID << "\n";
    waitForEnter(); 
}

// Update patient information by IC
void updatePatientInfo() {
    if (patients.empty()) {
        cout << "\n[ERROR] No patient records found. Please register a patient first.\n";
        waitForEnter();
        return;
    }

    string ic;
    int idx;

    cout << "\n----- UPDATE PATIENT INFORMATION -----\n";

    // Loop until valid IC is entered or user cancels
    while (true) {
        cout << "\nEnter Patient IC to update (or 0 to cancel): ";
        getline(cin, ic);

        if (ic == "0") {
            cout << "[System] Update cancelled.\n";
            waitForEnter();
            return;
        }

        idx = searchPatientByIC(ic);

        if (idx == -1) {
            cout << "[ERROR] Patient with this IC not found. Please try again or enter 0 to cancel.\n";
            continue;
        }

        // Valid patient found, exit loop
        break;
    }

    cout << "Current record:\n";
    displayPatientRecord(patients[idx]);

    cout << "\nWhich field do you want to update?\n";
    cout << "  1. IC Number\n";
    cout << "  2. Name\n";
    cout << "  3. Age\n";
    cout << "  4. Gender\n";
    cout << "  5. Phone Number\n";
    cout << "  6. Allergy History\n";
    cout << "  0. Cancel / Back\n";
    int choice = readValidatedInt("\nEnter your choice: ", 0, 6);

    switch (choice) {
        case 0: {
            cout << "[System] Update cancelled.\n";
            waitForEnter();
            break;
        }

        case 1: {
            string newIC;
            while (true) {
                newIC = readNonEmptyLine("\nEnter new IC Number (format: XXXXXX-XX-XXXX): ");
                
                if (!validateICFormat(newIC)) {
                    cout << "[ERROR] Invalid IC format. Please use format XXXXXX-XX-XXXX.\n";
                    continue;
                }

                bool duplicate = false;
                for (int i = 0; i < (int)patients.size(); i++) {
                    if (i != idx && patients[i].icNumber == newIC) { 
                        duplicate = true; 
                        break; 
                    }
                }

                if (duplicate) {
                    cout << "[ERROR] This IC number is already used by another patient.\n";
                    continue;
                }
                break;
                
            }

            patients[idx].icNumber = newIC;
            cout << "[System] IC number updated successfully.\n";
            break;
        }

        case 2: {
            string newName = readNonEmptyLine("\nEnter new name: ");
            patients[idx].name = newName;
            cout << "[System] Name updated successfully.\n";
            break;
        }

        case 3: {
            int newAge = readValidatedInt("\nEnter new age: ", 0, 150);
            patients[idx].age = newAge;
            cout << "[System] Age updated successfully.\n";
            break;
        }

        case 4: {
            char newGender = readValidatedGender();
            patients[idx].gender = newGender;
            cout << "[System] Gender updated successfully.\n";
            break;
        }

        case 5: {
            string newPhone = readValidatedPhone("\nEnter new Phone Number (e.g. 012-3456789): ");
            patients[idx].phone = newPhone;
            cout << "[System] Phone number updated successfully.\n";
            break;
        }

        case 6: {
            cout << "\nEnter new allergy history (or type None): ";
            string newAllergy;
            getline(cin, newAllergy);

            if (newAllergy == "None" || newAllergy == "none") {
                newAllergy = "";
            }

            patients[idx].allergyHistory = newAllergy;
            cout << "[System] Allergy history updated successfully.\n";
            break;
        }
    }

    // Only pause if an actual update was performed (choice != 0)
    if (choice != 0) {
        waitForEnter();
    }
}

// ============================================================
// MAIN MENU
// ============================================================
void patientManagementMenu() {
    int choice;

    do {        
        clearScreen();
        setColor(COLOR_DARK_BLUE);
        cout << "\n  ============================================================\n";
        cout << "  |                 PATIENT MANAGEMENT MODULE                |\n";
        cout << "  +----------------------------------------------------------+\n";
        cout << "  |                                                          |\n";
        
        setColor(COLOR_LIGHT_BLUE);
        cout << "  |   1.  Register New Patient                               |\n";
        cout << "  |   2.  Search Patient by IC                               |\n";
        cout << "  |   3.  Search Patient by Name                             |\n";
        cout << "  |   4.  Update Patient Information                         |\n";
        cout << "  |   5.  View All Patients                                  |\n";
        
        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        
        setColor(COLOR_RED);
        cout << "  |   6.  Back to Main Menu                                  |\n";
        
        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        cout << "  ============================================================\n";
        setColor(COLOR_RESET);
        
        choice = readValidatedInt("  Enter your choice: ", 1, 6);

        switch (choice) {
            case 1: 
                registerNewPatient(); 
                break;

            case 2: {
                if (patients.empty()) {
                    cout << "\n[ERROR] No patient records found. Please register a patient first.\n";
                    pressZeroToExit();
                    break;
                }

                string ic;
                int idx;

                while (true) {
                    cout << "\nEnter IC to search (or 0 to cancel): ";
                    getline(cin, ic);

                    if (ic == "0") {
                        cout << "[System] Search cancelled.\n";
                        waitForEnter();
                        break; 
                    }

                    idx = searchPatientByIC(ic);

                    if (idx == -1) {
                        cout << "[ERROR] Patient not found. Please try again.\n";
                        continue;
                    }

                    displayPatientRecord(patients[idx]);
                    pressZeroToExit();   // ✅ 仅查看记录后要求按0退出
                    break;
                }
                break;
            }

            case 3: {
                if (patients.empty()) {
                    cout << "\n[ERROR] No patient records found. Please register a patient first.\n";
                    pressZeroToExit();
                    break;
                }

                string name;
                int idx;

                while (true) {
                    cout << "\nEnter name to search (or 0 to cancel): ";
                    getline(cin, name);

                    if (name == "0") {
                        cout << "[System] Search cancelled.\n";
                        waitForEnter();
                        break;                     
                    }

                    idx = searchPatientByName(name);

                    if (idx == -1) {
                        cout << "[ERROR] Patient not found. Please try again or enter 0 to cancel.\n";
                        continue;
                    }

                    displayPatientRecord(patients[idx]);
                    pressZeroToExit();   // ✅ 查看记录后按0退出
                    break;
                }
                break;
            }

            case 4: 
                updatePatientInfo(); 
                break;

            case 5: 
                viewAllPatients();   
                break;

            case 6: 
                clearScreen(); 
                break;

            default: 
                cout << "[ERROR] Invalid choice. Please enter 1-6.\n";
                pressZeroToExit();
        }
    } while (choice != 6);
}