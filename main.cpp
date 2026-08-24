/*
    ============================================================
    Clinic Management System
    main.cpp — TEAM SHARED FILE
    ============================================================
    This is the single entry point for the whole system. It is a
    TEAM responsibility (System Architecture / Main menu &
    navigation / Shared data management per the assignment spec),
    not any one member's individual module.

    Swap "teammate_placeholders.h" below for the real headers once
    Chia Koh Suan / Pah Yan Han / Siow Pei En finish their modules.
    ============================================================
*/

#include <iostream>
#include "shared_data.h"
#include "teammate_placeholders.h"   // TODO: replace with real headers once ready
#include "billing_report.h"          // Soo Hui Xin's module
using namespace std;

bool login(vector<Staff>& staffList, string username, string password) {
    for (int i = 0; i < (int)staffList.size(); i++) {
        if (staffList[i].username == username && staffList[i].password == password) {
            return true;
        }
    }
    return false;
}

void displayIntroScreen() {
    cout << "========================================\n";
    cout << "     CLINIC MANAGEMENT SYSTEM\n";
    cout << "========================================\n\n";
}

int main() {
    // ---- Stage 1: shared data storage for the WHOLE system ----
    vector<Staff> staffList = { {"admin", "1234", "Admin"} };
    vector<Patient> patients;
    vector<Appointment> appointments;
    vector<Consultation> consultations;
    vector<Medicine> medicineInventory;
    vector<BillRecord> billRecords;

    displayIntroScreen();

    // ---- System Login (shared, applies to whole system) ----
    string username, password;
    bool loggedIn = false;
    int attempts = 0;
    do {
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;
        if (login(staffList, username, password)) {
            loggedIn = true;
            cout << "\nLogin successful! Welcome, " << username << ".\n";
        } else {
            attempts++;
            cout << "Invalid credentials. Attempts left: " << (MAX_LOGIN_ATTEMPTS - attempts) << "\n";
        }
    } while (!loggedIn && attempts < MAX_LOGIN_ATTEMPTS);

    if (!loggedIn) {
        cout << "\nToo many failed attempts. Exiting system.\n";
        return 0;
    }

    // ---- Main Menu: one option per MAJOR functional area ----
    int choice;
    do {
        cout << "\n============ MAIN MENU ============\n";
        cout << "1. Patient Management\n";
        cout << "2. Appointment & Queue Management\n";
        cout << "3. Consultation & Medicine Inventory\n";
        cout << "4. Billing & Reporting\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                patientManagementMenu(patients);
                break;
            case 2:
                appointmentQueueMenu(appointments, patients);
                break;
            case 3:
                consultationMedicineMenu(consultations, medicineInventory, patients);
                break;
            case 4:
                billingReportMenu(billRecords, patients, consultations, medicineInventory);
                break;
            case 0:
                cout << "Exiting system...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);

    cout << "\nThank you for using the Clinic Management System. Goodbye!\n";
    return 0;
}