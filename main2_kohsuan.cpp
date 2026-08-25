
 //===========================================================
 // main.cpp — Clinic Management System
 // Main menu & navigation / Login & authentication
 //============================================================

#include <iostream>
#include "clinic_shared.h"
#include "input_utils.h"
#include "login_module.h"
#include "patient_module.h"
using namespace std;


void displayIntroScreen() {
    cout << "======================================================\n";
    cout << "         TARUMT CLINIC MANAGEMENT SYSTEM\n";
    cout << "======================================================\n";
    cout << "  Welcome! This system helps manage patients,\n";
    cout << "  appointments, consultations, medicine stock,\n";
    cout << "  and billing for the clinic.\n";
    cout << "======================================================\n";
}


int main() {
    seedSharedData();          // 放入demo资料（含预设登入帐号，见clinic_shared.cpp）
    displayIntroScreen();
    authenticationGate();      // 登入成功前，程序会一直停在这个关卡

    int choice;
	// Use do-while loop to enter the main menu loop until the user chooses to exit
    do {
        cout << "\n===================== MAIN MENU =====================\n";
        cout << "1. Patient Management\n";
        cout << "2. Appointment & Queue Management\n";
        cout << "3. Consultation & Medicine Inventory\n";
        cout << "4. Billing & Reporting\n";
        cout << "5. Exit\n";
        cout << "=======================================================\n";
        choice = readValidatedInt("Enter your choice: ", 1, 5);

        switch (choice) {
            case 1: 
                patientManagementMenu();      
                break;
            case 2: 
                // appointmentManagementMenu();  
                break;
            case 3: 
                // consultationMedicineMenu();   
                break;
            case 4: 
                // billingReportMenu(billRecords); 
                break;  
            case 5: 
				cout << "\n[System] Logging out and returning to login screen...\n";
                authenticationGate();
                break;
            default: 
                cout << "[ERROR] Invalid choice. Please enter 1-5.\n";
        }
    } while (choice != 5);

    return 0;
}
