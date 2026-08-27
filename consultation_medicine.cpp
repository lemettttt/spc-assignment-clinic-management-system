#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <sstream>
#include "consultation_medicine.h"
#include "input_utils.h"

using namespace std;


// ============================================================
// HELPER FUNCTION: Find Patient
// ============================================================

int findPatientIndex(
    const vector<Patient>& patients,
    int patientID
) {
    for (int i = 0; i < patients.size(); i++) {
        if (patients[i].patientID == patientID) {
            return i;
        }
    }

    return -1;
}


// ============================================================
// HELPER FUNCTION: Find Medicine
// ============================================================

int findMedicineIndex(
    const vector<Medicine>& medicineInventory,
    int medicineID
) {
    for (int i = 0; i < medicineInventory.size(); i++) {
        if (medicineInventory[i].medicineID == medicineID) {
            return i;
        }
    }

    return -1;
}


// ============================================================
// HELPER FUNCTION: Find Consultation
// ============================================================

int findConsultationIndex(
    const vector<Consultation>& consultations,
    int consultationID
) {
    for (int i = 0; i < consultations.size(); i++) {
        if (consultations[i].consultationID == consultationID) {
            return i;
        }
    }

    return -1;
}


// ============================================================
// 1. RECORD CONSULTATION
// ============================================================

void recordConsultation(
    vector<Consultation>& consultations,
    const vector<Patient>& patients
) {
    Consultation newConsultation;

    cout << "\n========== RECORD CONSULTATION ==========\n";

    newConsultation.consultationID = getPositiveInt("Enter Consultation ID: ");

    // Check duplicate consultation ID
    if (findConsultationIndex(
            consultations,
            newConsultation.consultationID
        ) != -1) {

        cout << "Error: Consultation ID already exists.\n";
        pressZeroToContinue();
        return;
    }

    newConsultation.patientID = getPositiveInt("Enter Patient ID: ");

    int patientIndex = findPatientIndex(
        patients,
        newConsultation.patientID
    );

    // Validate Patient ID
    if (patientIndex == -1) {
        cout << "Error: Patient ID does not exist.\n";
        pressZeroToContinue();
        return;
    }

    cout << "Patient Name: " << patients[patientIndex].name << endl;

    newConsultation.doctorID = getPositiveInt("Enter Doctor ID: ");

    // Set doctorType (required by clinic_shared.h)
    newConsultation.doctorType = "General";   // You can customize later

    newConsultation.diagnosis = getNonEmptyString("Enter Diagnosis: ");

    // Use consultationDate instead of date
    newConsultation.consultationDate = getNonEmptyString("Enter Consultation Date (DD/MM/YYYY): ");

    // Use followUpDate instead of followUpDays
    cout << "Enter Follow-up Date (DD/MM/YYYY) or 'None' if not required: ";
    string followUp = getNonEmptyString("");   // getNonEmptyString requires prompt, but we can bypass
    // Actually, getNonEmptyString always prompts, so better to use a separate function
    // I'll use a custom input method below:

    // Let's use a simple getline loop to allow "None"
    string followUpInput;
    do {
        cout << "Enter Follow-up Date (DD/MM/YYYY) or 'None' if not required: ";
        getline(cin, followUpInput);
        if (!followUpInput.empty()) {
            break;
        }
        cout << "Input cannot be empty. Please enter a value.\n";
    } while (true);
    newConsultation.followUpDate = followUpInput;

    consultations.push_back(newConsultation);

    cout << "\nConsultation recorded successfully.\n";
    pressZeroToContinue();
}


// ============================================================
// 2. VIEW MEDICAL HISTORY
// ============================================================

void viewMedicalHistory(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
) {
    cout << "\n========== VIEW MEDICAL HISTORY ==========\n";

    int patientID = getPositiveInt("Enter Patient ID: ");

    int patientIndex = findPatientIndex(
        patients,
        patientID
    );

    if (patientIndex == -1) {
        cout << "Error: Patient ID does not exist.\n";
        pressZeroToContinue();
        return;
    }

    cout << "\nPatient ID: " << patients[patientIndex].patientID << endl;
    cout << "Patient Name: " << patients[patientIndex].name << endl;
    cout << "Allergy History: " << patients[patientIndex].allergyHistory << endl;

    bool found = false;

    cout << "\n---------- Consultation History ----------\n";

    for (const Consultation& consultation : consultations) {

        if (consultation.patientID == patientID) {

            found = true;

            cout << "\nConsultation ID: " << consultation.consultationID << endl;
            cout << "Doctor ID: " << consultation.doctorID << endl;
            cout << "Diagnosis: " << consultation.diagnosis << endl;
            cout << "Date: " << consultation.consultationDate << endl;   // Changed

            if (consultation.followUpDate != "None" && !consultation.followUpDate.empty()) {
                cout << "Follow-up Date: " << consultation.followUpDate << endl;
            } else {
                cout << "Follow-up: Not required\n";
            }

            cout << "------------------------------------------\n";
        }
    }
    if (!found) {
        cout << "No consultation history found.\n";
    }
    pressZeroToContinue();
}


// ============================================================
// 3. CHECK STOCK AVAILABILITY
// ============================================================

bool checkStockAvailability(
    const vector<Medicine>& medicineInventory,
    int medicineID,
    int quantity
) {
    int index = findMedicineIndex(
        medicineInventory,
        medicineID
    );

    if (index == -1) {
        cout << "Error: Medicine ID does not exist.\n";
        return false;
    }

    if (quantity <= 0) {
        cout << "Error: Quantity must be greater than 0.\n";
        return false;
    }

    if (medicineInventory[index].quantity < quantity) {   // Changed: stockQty -> quantity

        cout << "Insufficient stock.\n";
        cout << "Available quantity: " << medicineInventory[index].quantity << endl;
        return false;
    }

    return true;
}


// ============================================================
// 4. PRESCRIBE MEDICINE
// ============================================================

void prescribeMedicine(
    vector<Medicine>& medicineInventory
) {
    int medicineID;
    int quantity;

    cout << "\n========== PRESCRIBE MEDICINE ==========\n";

    if (medicineInventory.empty()) {
        cout << "No medicine records available for prescription.\n";
        pressZeroToContinue();
        return;
    }

    displayMedicineList(medicineInventory);

    medicineID = getPositiveInt("\nEnter Medicine ID: ");
    quantity = getPositiveInt("Enter Quantity: ");

    if (!checkStockAvailability(
            medicineInventory,
            medicineID,
            quantity
        )) {

        cout << "Medicine prescription failed.\n";
        pressZeroToContinue();
        return;
    }

    int index = findMedicineIndex(
        medicineInventory,
        medicineID
    );

    medicineInventory[index].quantity -= quantity;   // Changed: stockQty -> quantity

    cout << "\nMedicine prescribed successfully.\n";
    cout << "Medicine: " << medicineInventory[index].medicineName << endl;   // Changed: name -> medicineName
    cout << "Quantity Prescribed: " << quantity << endl;
    cout << "Remaining Stock: " << medicineInventory[index].quantity << endl; // Changed

    pressZeroToContinue();
}


// ============================================================
// 5. DISPLAY MEDICINE LIST
// ============================================================

void displayMedicineList(
    const vector<Medicine>& medicineInventory
) {
    if (medicineInventory.empty()) {
        cout << "\nNo medicine records found.\n";
        return;
    }

    cout << "\n================ MEDICINE LIST ================\n";

    cout << left
         << setw(10) << "ID"
         << setw(25) << "Medicine Name"
         << setw(12) << "Stock"
         << setw(12) << "Price"
         << setw(15) << "Expiry Date"
         << endl;

    cout << string(74, '-') << endl;

    for (const Medicine& medicine : medicineInventory) {

        cout << left
             << setw(10) << medicine.medicineID
             << setw(25) << medicine.medicineName   // Changed: name -> medicineName
             << setw(12) << medicine.quantity       // Changed: stockQty -> quantity
             << setw(12) << fixed << setprecision(2) << medicine.price
             << setw(15) << medicine.expiryDate
             << endl;
    }
}


// ============================================================
// 6. UPDATE MEDICINE STOCK
// ============================================================

void updateMedicineStock(
    vector<Medicine>& medicineInventory
) {
    int medicineID;
    int quantity;

    cout << "\n========== UPDATE MEDICINE STOCK ==========\n";

    medicineID = getPositiveInt("Enter Medicine ID: ");

    int index = findMedicineIndex(
        medicineInventory,
        medicineID
    );

    if (index == -1) {
        cout << "Medicine not found.\n";
        pressZeroToContinue();
        return;
    }

    quantity = getPositiveInt("Enter quantity to add: ");

    medicineInventory[index].quantity += quantity;   // Changed: stockQty -> quantity

    cout << "\nMedicine stock updated successfully.\n";
    cout << "New Stock Quantity: " << medicineInventory[index].quantity << endl; // Changed
    pressZeroToContinue();
}


// ============================================================
// 7. ADD NEW MEDICINE
// ============================================================

void addNewMedicine(
    vector<Medicine>& medicineInventory
) {
    Medicine newMedicine;

    cout << "\n========== ADD NEW MEDICINE ==========\n";

    newMedicine.medicineID = getPositiveInt("Enter Medicine ID: ");

    // Check duplicate Medicine ID
    if (findMedicineIndex(
            medicineInventory,
            newMedicine.medicineID
        ) != -1) {

        cout << "Error: Medicine ID already exists.\n";
        pressZeroToContinue();
        return;
    }

    newMedicine.medicineName = getNonEmptyString("Enter Medicine Name: ");   // Changed: name -> medicineName

    newMedicine.quantity = getNonNegativeInt("Enter Stock Quantity: ");      // Changed: stockQty -> quantity

    newMedicine.price = getNonNegativeDouble("Enter Medicine Price: RM ");

    newMedicine.expiryDate = getNonEmptyString("Enter Expiry Date (DD/MM/YYYY): ");

    medicineInventory.push_back(newMedicine);

    cout << "\nNew medicine added successfully.\n";
    pressZeroToContinue();
}


// ============================================================
// 8. SEARCH MEDICINE
// ============================================================

void searchMedicine(
    const vector<Medicine>& medicineInventory
) {
    int medicineID;

    cout << "\n========== SEARCH MEDICINE ==========\n";

    medicineID = getPositiveInt("Enter Medicine ID: ");

    int index = findMedicineIndex(
        medicineInventory,
        medicineID
    );

    if (index == -1) {
        cout << "Medicine not found.\n";
        pressZeroToContinue();
        return;
    }

    const Medicine& medicine = medicineInventory[index];

    cout << "\nMedicine Found\n";
    cout << "Medicine ID: " << medicine.medicineID << endl;
    cout << "Medicine Name: " << medicine.medicineName << endl;   // Changed
    cout << "Stock Quantity: " << medicine.quantity << endl;      // Changed
    cout << "Price: RM " << fixed << setprecision(2) << medicine.price << endl;
    cout << "Expiry Date: " << medicine.expiryDate << endl;

    pressZeroToContinue();
}


// ============================================================
// 9. DELETE MEDICINE
// ============================================================

void deleteMedicine(
    vector<Medicine>& medicineInventory
) {
    int medicineID;
    char confirm;

    cout << "\n========== DELETE MEDICINE ==========\n";

    medicineID = getPositiveInt("Enter Medicine ID: ");

    int index = findMedicineIndex(
        medicineInventory,
        medicineID
    );

    if (index == -1) {
        cout << "Medicine not found.\n";
        pressZeroToContinue();
        return;
    }

    cout << "Medicine: " << medicineInventory[index].medicineName << endl;   // Changed

    confirm = getYesNo("Are you sure you want to delete this medicine? (Y/N): ");

    if (confirm == 'Y' || confirm == 'y') {

        medicineInventory.erase(
            medicineInventory.begin() + index
        );

        cout << "Medicine deleted successfully.\n";
    }
    else {
        cout << "Delete operation cancelled.\n";
    }
    pressZeroToContinue();
}


// ============================================================
// 10. LOW STOCK WARNING
// ============================================================

void displayLowStockWarning(
    const vector<Medicine>& medicineInventory,
    int threshold
) {
    cout << "\n========== LOW STOCK WARNING ==========\n";

    bool found = false;

    for (const Medicine& medicine : medicineInventory) {

        if (medicine.quantity < threshold) {   // Changed: stockQty -> quantity

            found = true;

            cout << "\nWARNING: LOW STOCK\n";
            cout << "Medicine: " << medicine.medicineName << endl;   // Changed
            cout << "Current Stock: " << medicine.quantity << endl;  // Changed
            cout << "Minimum Level: " << threshold << endl;
        }
    }

    if (!found) {
        cout << "All medicine stock levels are sufficient.\n";
    }
    pressZeroToContinue();
}


// ============================================================
// 11. CHECK EXPIRY DATE
// ============================================================

void checkExpiryDate(
    const vector<Medicine>& medicineInventory
) {
    cout << "\n========== MEDICINE EXPIRY CHECK ==========\n";

    if (medicineInventory.empty()) {
        cout << "No medicine records found.\n";
        pressZeroToContinue();
        return;
    }

    cout << "Current implementation displays all expiry dates.\n";
    cout << "Automatic date comparison can be added next.\n\n";

    for (const Medicine& medicine : medicineInventory) {

        cout << "Medicine: " << medicine.medicineName << endl;   // Changed
        cout << "Expiry Date: " << medicine.expiryDate << endl;
        cout << "--------------------------------\n";
    }
    pressZeroToContinue();
}


// ============================================================
// 12. FOLLOW-UP REMINDER (updated to use followUpDate)
// ============================================================

void displayFollowUpReminder(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
) {
    cout << "\n========== FOLLOW-UP REMINDERS ==========\n";

    bool found = false;

    for (const Consultation& consultation : consultations) {

        if (consultation.followUpDate != "None" && !consultation.followUpDate.empty()) {

            int patientIndex = findPatientIndex(
                patients,
                consultation.patientID
            );

            found = true;

            cout << "\nPatient ID: " << consultation.patientID << endl;

            if (patientIndex != -1) {
                cout << "Patient Name: " << patients[patientIndex].name << endl;
            }

            cout << "Consultation Date: " << consultation.consultationDate << endl;   // Changed
            cout << "Follow-up Date: " << consultation.followUpDate << endl;
            cout << "Diagnosis: " << consultation.diagnosis << endl;
            cout << "--------------------------------\n";
        }
    }

    if (!found) {
        cout << "No follow-up reminders found.\n";
    }
    pressZeroToContinue();
}


// ============================================================
// MAIN MODULE MENU
// ============================================================

void consultationMedicineMenu(
    vector<Consultation>& consultations,
    vector<Medicine>& medicineInventory,
    vector<Patient>& patients
) {
    int choice;

    do {

        cout << "\n";
        cout << "=================================================\n";
        cout << "  CONSULTATION & MEDICINE INVENTORY MANAGEMENT\n";
        cout << "=================================================\n";

        cout << "1.  Record Consultation\n";
        cout << "2.  View Medical History\n";
        cout << "3.  Prescribe Medicine\n";
        cout << "4.  Display Medicine List\n";
        cout << "5.  Update Medicine Stock\n";
        cout << "6.  Add New Medicine\n";
        cout << "7.  Search Medicine\n";
        cout << "8.  Delete Medicine\n";
        cout << "9.  Check Low Stock Warning\n";
        cout << "10. Check Medicine Expiry Dates\n";
        cout << "11. Display Follow-up Reminders\n";
        cout << "12. Back to Main Menu\n";

        choice = getValidatedMenuChoice(
            "\nEnter your choice: ",
            1,
            12
        );

        switch (choice) {

            case 1:
                recordConsultation(consultations, patients);
                break;

            case 2:
                viewMedicalHistory(consultations, patients);
                break;

            case 3:
                prescribeMedicine(medicineInventory);
                break;

            case 4:
                displayMedicineList(medicineInventory);
                pressZeroToContinue();
                break;

            case 5:
                updateMedicineStock(medicineInventory);
                break;

            case 6:
                addNewMedicine(medicineInventory);
                break;

            case 7:
                searchMedicine(medicineInventory);
                break;

            case 8:
                deleteMedicine(medicineInventory);
                break;

            case 9:
                displayLowStockWarning(medicineInventory, 10);
                break;

            case 10:
                checkExpiryDate(medicineInventory);
                break;

            case 11:
                displayFollowUpReminder(consultations, patients);
                break;

            case 12:
                cout << "\nReturning to Main Menu...\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 12);
}