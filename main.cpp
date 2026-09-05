#include <iostream>
#include <cctype>
#include "clinic_shared.h"
#include "input_utils.h"
#include "login_module.h"
#include "patient_module.h"
#include "appointment_queuemanagement.h"
#include "consultation_medicine.h"
#include "billing_report.h"
using namespace std;

// ============================================================
// Welcome Screen
// ============================================================

void displayIntroScreen() {
    cout << "\n";
    
    setColor(COLOR_DARK_BLUE);
    cout << "  ============================================================\n";
    cout << "  |                                                          |\n";
    
    setColor(COLOR_BRIGHT_BLUE);
    cout << "  |         TARUMT CLINIC MANAGEMENT SYSTEM                  |\n";
    
    setColor(COLOR_DARK_BLUE);
    cout << "  |                                                          |\n";
    
    setColor(COLOR_CYAN);
    cout << "  |    Welcome! This system helps manage patients,           |\n";
    setColor(COLOR_LIGHT_BLUE);
    cout << "  |    appointments, consultations, medicine stock,          |\n";
    setColor(COLOR_CYAN);
    cout << "  |    and billing for the clinic.                           |\n";
    
    setColor(COLOR_DARK_BLUE);
    cout << "  |                                                          |\n";
    cout << "  ============================================================\n";
    
    setColor(COLOR_RESET);
    cout << "\n";
}

// ============================================================
// MAIN MENU
// ============================================================

void displayMainMenu() {
    cout << "\n";
    setColor(COLOR_DARK_BLUE);
    cout << "  ============================================================\n";
    cout << "  |                     MAIN MENU                            |\n";
    cout << "  +----------------------------------------------------------+\n";
    cout << "  |                                                          |\n";
    
    setColor(COLOR_LIGHT_BLUE);
    cout << "  |   1.  Patient Management                                 |\n";
    cout << "  |   2.  Appointment & Queue Management                     |\n";
    cout << "  |   3.  Consultation & Medicine Inventory                  |\n";
    cout << "  |   4.  Billing & Reporting                                |\n";
    
    setColor(COLOR_DARK_BLUE);
    cout << "  |                                                          |\n";
    
    setColor(COLOR_RED);
    cout << "  |   5.  Logout                                             |\n";
    
    setColor(COLOR_DARK_BLUE);
    cout << "  |                                                          |\n";
    cout << "  ============================================================\n";
    
    setColor(COLOR_RESET);
    cout << "\n";
}

int main() {
    clearScreen();
    seedSharedData();
    displayIntroScreen();
    authenticationGate();

    int choice;
    do {
        displayMainMenu();
        choice = readValidatedInt("  Enter your choice: ", 1, 5);

        switch (choice) {
            case 1:
                patientManagementMenu();
                break;
            case 2:
                appointmentQueueMenu(appointments, patients);
                break;
            case 3:
                consultationMedicineMenu(consultations, medicines, patients, doctors);
                break;
            case 4:
                billingReportMenu(billRecords, patients, consultations, medicines);
                break;
            case 5:
                cout << "\n  [System] Logging out and returning to login screen...\n";
                waitForEnter();
                clearScreen();
                authenticationGate();   // Return to login screen
                // Reset choice to avoid exiting the outer loop
                choice = 0;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice. Please enter 1-5.\n";
                pressZeroToExit();
        }
    } while (choice != 5);

    return 0;
}