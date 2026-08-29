#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <sstream>
#include "consultation_medicine.h"
#include "input_utils.h"

using namespace std;

// ============================================================
// HELPER FUNCTION: Find Doctor
// ============================================================

int findDoctorIndex(
    const vector<Doctor>& doctors,
    int doctorID
) {
    for (int i = 0; i < doctors.size(); i++) {
        if (doctors[i].doctorID == doctorID) {
            return i;
        }
    }
    return -1;
}

// ============================================================
// HELPER FUNCTION: Display Doctor List
// ============================================================

void displayDoctorList(const vector<Doctor>& doctors) {
    if (doctors.empty()) {
        cout << "\nNo doctors available.\n";
        return;
    }
    
    cout << "\n========== AVAILABLE DOCTORS ==========\n";
    cout << left
         << setw(12) << "Doctor ID"
         << setw(25) << "Doctor Name"
         << setw(15) << "Specialization"
         << endl;
    cout << string(52, '-') << endl;
    
    for (const Doctor& doc : doctors) {
        cout << left
             << setw(12) << doc.doctorID
             << setw(25) << doc.name
             << setw(15) << doc.specialization
             << endl;
    }
    cout << "========================================\n";
}

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

    // Auto-generate Consultation ID
    newConsultation.consultationID = nextConsultationID++;
    cout << "Auto-generated Consultation ID: " << newConsultation.consultationID << endl;

    newConsultation.patientID = getPositiveInt("Enter Patient ID: ");

    int patientIndex = findPatientIndex(
        patients,
        newConsultation.patientID
    );

    // Validate Patient ID
    if (patientIndex == -1) {
        cout << "Error: Patient ID does not exist.\n";
        pressZeroToContinue();
        // Rollback the consultation ID if registration fails
        nextConsultationID--;
        return;
    }

    cout << "Patient Name: " << patients[patientIndex].name << endl;

    // Display available doctors
    displayDoctorList(doctors);

    // Get and validate Doctor ID
    newConsultation.doctorID = getPositiveInt("Enter Doctor ID: ");
    
    int doctorIndex = findDoctorIndex(doctors, newConsultation.doctorID);
    if (doctorIndex == -1) {
        cout << "Error: Doctor ID does not exist. Please enter a valid Doctor ID from the list.\n";
        pressZeroToContinue();
        nextConsultationID--;  // Rollback
        return;
    }

    // Set doctorType based on selected doctor's specialization
    newConsultation.doctorType = doctors[doctorIndex].specialization;

    newConsultation.diagnosis = getNonEmptyString("Enter Diagnosis: ");

    // Use validated date from input_utils.h - allows past, present, or future
    newConsultation.consultationDate = getValidatedDate("Enter Consultation Date (DD/MM/YYYY): ");

    // Follow-up date input - can be past, present, or future
    string followUpInput;
    do {
        cout << "Enter Follow-up Date (DD/MM/YYYY) or 'None' if not required: ";
        getline(cin, followUpInput);
        if (!followUpInput.empty()) {
            break;
        }
        cout << "Input cannot be empty. Please enter a value.\n";
    } while (true);
    
    // Validate follow-up date if not "None"
    if (followUpInput != "None" && followUpInput != "none") {
        while (!isValidDate(followUpInput)) {
            cout << "Invalid date format. Please use DD/MM/YYYY (e.g., 25/12/2026) or 'None'.\n";
            cout << "Enter Follow-up Date (DD/MM/YYYY) or 'None' if not required: ";
            getline(cin, followUpInput);
        }
    }
    newConsultation.followUpDate = followUpInput;

    consultations.push_back(newConsultation);

    cout << "\nConsultation recorded successfully.\n";
    cout << "Consultation ID: " << newConsultation.consultationID << endl;
    cout << "Doctor: " << doctors[doctorIndex].name << " (" << doctors[doctorIndex].specialization << ")\n";
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
            
            // Find doctor name
            int doctorIdx = findDoctorIndex(doctors, consultation.doctorID);
            string doctorName = (doctorIdx != -1) ? doctors[doctorIdx].name : "Unknown Doctor";

            cout << "\nConsultation ID: " << consultation.consultationID << endl;
            cout << "Doctor: " << doctorName << " (ID: " << consultation.doctorID << ")" << endl;
            cout << "Diagnosis: " << consultation.diagnosis << endl;
            cout << "Date: " << consultation.consultationDate << endl;

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

    if (medicineInventory[index].quantity < quantity) {

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

    medicineInventory[index].quantity -= quantity;

    cout << "\nMedicine prescribed successfully.\n";
    cout << "Medicine: " << medicineInventory[index].medicineName << endl;
    cout << "Quantity Prescribed: " << quantity << endl;
    cout << "Remaining Stock: " << medicineInventory[index].quantity << endl;

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
             << setw(25) << medicine.medicineName
             << setw(12) << medicine.quantity
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

    medicineInventory[index].quantity += quantity;

    cout << "\nMedicine stock updated successfully.\n";
    cout << "New Stock Quantity: " << medicineInventory[index].quantity << endl;
    pressZeroToContinue();
}


// ============================================================
// 7. ADD NEW MEDICINE - AUTO-GENERATE MEDICINE ID WITH EXPIRY VALIDATION
// ============================================================

void addNewMedicine(
    vector<Medicine>& medicineInventory
) {
    Medicine newMedicine;

    cout << "\n========== ADD NEW MEDICINE ==========\n";

    // Auto-generate Medicine ID using nextMedicineID from clinic_shared.cpp
    newMedicine.medicineID = nextMedicineID++;
    cout << "Auto-generated Medicine ID: " << newMedicine.medicineID << endl;

    // Check if ID already exists (safety check)
    if (findMedicineIndex(medicineInventory, newMedicine.medicineID) != -1) {
        cout << "Error: Medicine ID already exists. Please restart.\n";
        pressZeroToContinue();
        nextMedicineID--;  // Rollback
        return;
    }

    newMedicine.medicineName = getNonEmptyString("Enter Medicine Name: ");

    newMedicine.quantity = getNonNegativeInt("Enter Stock Quantity: ");

    newMedicine.price = getNonNegativeDouble("Enter Medicine Price: RM ");

    // FIXED: Use expiry date validation - must be in the future
    newMedicine.expiryDate = getValidatedExpiryDate("Enter Expiry Date (DD/MM/YYYY): ");

    medicineInventory.push_back(newMedicine);

    cout << "\nNew medicine added successfully.\n";
    cout << "Medicine ID: " << newMedicine.medicineID << endl;
    cout << "Expiry Date: " << newMedicine.expiryDate << endl;
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
    cout << "Medicine Name: " << medicine.medicineName << endl;
    cout << "Stock Quantity: " << medicine.quantity << endl;
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

    cout << "Medicine: " << medicineInventory[index].medicineName << endl;

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

        if (medicine.quantity < threshold) {

            found = true;

            cout << "\nWARNING: LOW STOCK\n";
            cout << "Medicine: " << medicine.medicineName << endl;
            cout << "Current Stock: " << medicine.quantity << endl;
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

    cout << left
         << setw(25) << "Medicine Name"
         << setw(15) << "Expiry Date"
         << setw(12) << "Status"
         << endl;
    cout << string(52, '-') << endl;

    // Get today's date for comparison
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int todayYear = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay = localTime->tm_mday;

    for (const Medicine& medicine : medicineInventory) {

        cout << left << setw(25) << medicine.medicineName;

        // Check if expiry date is valid format before parsing
        bool isExpired = false;
        if (isValidDate(medicine.expiryDate)) {
            int day = stoi(medicine.expiryDate.substr(0, 2));
            int month = stoi(medicine.expiryDate.substr(3, 2));
            int year = stoi(medicine.expiryDate.substr(6, 4));

            if (year < todayYear ||
                (year == todayYear && month < todayMonth) ||
                (year == todayYear && month == todayMonth && day < todayDay)) {
                isExpired = true;
            }
        }

        cout << setw(15) << medicine.expiryDate;

        if (isExpired) {
            cout << "EXPIRED!";
        } else {
            cout << "Valid";
        }
        cout << endl;
    }
    pressZeroToContinue();
}


// ============================================================
// 12. FOLLOW-UP REMINDER
// ============================================================

void displayFollowUpReminder(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
) {
    cout << "\n========== FOLLOW-UP REMINDERS ==========\n";

    bool found = false;

    // Get today's date for comparison
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int todayYear = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay = localTime->tm_mday;

    cout << left
         << setw(12) << "Patient ID"
         << setw(20) << "Patient Name"
         << setw(20) << "Doctor"
         << setw(15) << "Follow-up Date"
         << setw(12) << "Status"
         << endl;
    cout << string(77, '-') << endl;

    for (const Consultation& consultation : consultations) {

        if (consultation.followUpDate != "None" && !consultation.followUpDate.empty()) {

            int patientIndex = findPatientIndex(
                patients,
                consultation.patientID
            );
            
            int doctorIdx = findDoctorIndex(doctors, consultation.doctorID);

            found = true;

            string patientName = (patientIndex != -1) ? patients[patientIndex].name : "Unknown";
            string doctorName = (doctorIdx != -1) ? doctors[doctorIdx].name : "Unknown";

            // Check if follow-up date is overdue
            bool isOverdue = false;
            if (isValidDate(consultation.followUpDate)) {
                int day = stoi(consultation.followUpDate.substr(0, 2));
                int month = stoi(consultation.followUpDate.substr(3, 2));
                int year = stoi(consultation.followUpDate.substr(6, 4));

                if (year < todayYear ||
                    (year == todayYear && month < todayMonth) ||
                    (year == todayYear && month == todayMonth && day < todayDay)) {
                    isOverdue = true;
                }
            }

            cout << left
                 << setw(12) << consultation.patientID
                 << setw(20) << patientName
                 << setw(20) << doctorName
                 << setw(15) << consultation.followUpDate;

            if (isOverdue) {
                cout << "OVERDUE!";
            } else {
                cout << "Pending";
            }
            cout << endl;
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