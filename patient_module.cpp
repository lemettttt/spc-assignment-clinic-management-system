/*
 * ============================================================
 *  patient_module.cpp — Patient Management Module 
 *  Chia Koh Suan
 * ============================================================
 */
#include <iostream>
#include <iomanip>
#include <cctype>
#include "patient_module.h"
#include "input_utils.h"
using namespace std;

// STEP 1：validateICFormat() —— check IC format：XXXXXX-XX-XXXX
bool validateICFormat(const string& ic) {
	
    // Check the length of the IC
    if (ic.length() != IC_LENGTH) {
        return false;
    }

    // Check each character in the IC
    for (int i = 0; i < IC_LENGTH; i++) {

        if (i == 6 || i == 9) {
            // Position 6 and 9 must be '-'
            if (ic[i] != '-') {
                return false;
            }
        }
        else {
            // Other positions must be digits
            if (!isdigit(ic[i])) {
                return false;
            }
        }
    }

    return true;
}

// STEP 2：checkDuplicateIC() - - check if the IC number already exists in the patients vector
bool checkDuplicateIC(const string& ic) {

    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].icNumber == ic)
            return true;    // true = 找到了重复 IC
    }

    return false;       // false = 没有重复
}

// STEP 3：searchPatientByIC()
int searchPatientByIC(const string& ic) {

    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].icNumber == ic)
            return i;
    }

    return -1;  // 没有找到
}

// STEP 4：searchPatientByName()
int searchPatientByName(const string& name) {

    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].name == name)
            return i;
    }

    return -1;  // 没有找到
}

// STEP 5：displayPatientRecord()
void displayPatientRecord(const Patient& p) {

    cout << "\n---------------------------------------------\n";
    cout << left << setw(16) << "Patient ID"  << ": " << p.patientID << "\n";
    cout << left << setw(16) << "IC Number"   << ": " << p.icNumber << "\n";
    cout << left << setw(16) << "Name"        << ": " << p.name << "\n";
    cout << left << setw(16) << "Age"         << ": " << p.age << "\n";
    cout << left << setw(16) << "Gender"      << ": " << p.gender << "\n";
    cout << left << setw(16) << "Phone"       << ": " << p.phone << "\n";
    cout << left << setw(16) << "Allergies"   << ": ";

	// If allergyHistory is empty, display "None"
    if (p.allergyHistory.empty()) {
        cout << "None";
    }
    else {
        cout << p.allergyHistory;
    }

    cout << "\n";
    cout << "---------------------------------------------\n";
}

// STEP 6：viewAllPatients()
void viewAllPatients() {

    cout << "\n===================== ALL PATIENTS =====================\n";

    if (patients.empty()) {
        cout << "  (No patient records found)\n";
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

		// If allergyHistory is empty, display "None"
        if (p.allergyHistory.empty()) {
            cout << "None";
        }
        else {
            cout << p.allergyHistory;
        }

        cout << "\n";
    }
    cout << "==========================================================\n";
}

// STEP 7：registerNewPatient()
void registerNewPatient() {
    string ic, name, phone, allergy;
    int age;
    char gender;

    cout << "\n----- REGISTER NEW PATIENT -----\n";

	// Check if the patients vector is full (max 1000 patients)
    while (true) {

        cout << "\nEnter IC Number (format: XXXXXX-XX-XXXX): ";
        getline(cin, ic);

        if (!validateICFormat(ic)) {
            cout << "[ERROR] Invalid IC format. Please use format XXXXXX-XX-XXXX.\n";
            continue;
        }
        break;
    }

	    // Check for duplicate IC before proceeding with registration
        if (checkDuplicateIC(ic)) {
            int idx = searchPatientByIC(ic);
            cout << "[ERROR] Duplicate IC. Already registered as Patient ID " << patients[idx].patientID << " (" << patients[idx].name << ").\n";
            cout << "[System] Registration cancelled.\n";
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

		// Create a new Patient object and add it to the patients vector
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
}

// STEP 8：updatePatientInfo()
void updatePatientInfo() {

    // 如果一个病人都还没有，就不用继续问IC了
    if (patients.empty()) {
        cout << "\n[ERROR] No patient records found. Please register a patient first.\n";
        return;
    }

    string ic;
    cout << "\n----- UPDATE PATIENT INFORMATION -----\n";
    cout << "\nEnter Patient IC to update: ";
    getline(cin, ic);

    int idx = searchPatientByIC(ic);
    if (idx == -1) {
        cout << "[ERROR] Patient with this IC not found.\n";
        return;
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
    cout << "  7. Cancel\n";
    int choice = readValidatedInt("\nEnter your choice: ", 1, 7);

	// used switch statement to update the selected field based on user choice
    switch (choice) {

        case 1: {
            string newIC;

			// Check for duplicate IC before updating
            while (true) {
                newIC = readNonEmptyLine("\nEnter new IC Number (format: XXXXXX-XX-XXXX): ");
                
                if (!validateICFormat(newIC)) {
                    cout << "[ERROR] Invalid IC format. Please use format XXXXXX-XX-XXXX.\n";
                    continue;
                }

				// Check if the new IC is already used by another patient
                bool duplicate = false;
                for (int i = 0; i < (int)patients.size(); i++) {

					// If the index is not the current patient being updated and the IC matches, it's a duplicate
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

			// Update the IC number for the selected patient
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

        case 7:
            cout << "[System] Update cancelled.\n";
            break;
        }
}

// STEP 9：patientManagementMenu()
void patientManagementMenu() {

    int choice;

    do {
        cout << "\n===== PATIENT MANAGEMENT MODULE =====\n";
        cout << "1. Register New Patient\n";
        cout << "2. Search Patient by IC\n";
        cout << "3. Search Patient by Name\n";
        cout << "4. Update Patient Information\n";
        cout << "5. View All Patients\n";
        cout << "6. Back to Main Menu\n";
        cout << "======================================\n";
        choice = readValidatedInt("Enter your choice: ", 1, 6);

        switch (choice) {
            case 1: 
                registerNewPatient(); 
                break;

            case 2: {

                if (patients.empty()) {
                    cout << "\n[ERROR] No patient records found. Please register a patient first.\n";
                    break;
                }

                string ic = readNonEmptyLine("\nEnter IC to search: ");
                int idx = searchPatientByIC(ic);

				// If the patient is not found, display an error message; otherwise, display the patient's record
                if (idx == -1) {
                    cout << "[ERROR] Patient not found.\n";
                } else {
                    displayPatientRecord(patients[idx]);
                }
                break;

            }

            case 3: {
                if (patients.empty()) {
                    cout << "\n[ERROR] No patient records found. Please register a patient first.\n";
                    break;
                }

                string name = readNonEmptyLine("\nEnter name to search: ");
                int idx = searchPatientByName(name);
                if (idx == -1) {
                    cout << "[ERROR] Patient not found.\n";
                } else {
                    displayPatientRecord(patients[idx]);
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
                cout << "\n[System] Returning to Main Menu...\n"; 
                break;

            default: 
                cout << "[ERROR] Invalid choice. Please enter 1-6.\n";
        }
    } while (choice != 6);
}
