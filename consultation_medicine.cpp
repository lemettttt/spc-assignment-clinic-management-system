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
    cout << "========================================\n\n";
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

    // Get and validate Patient ID with retry option
    while (true) {
        int patientID = getNonNegativeInt("Enter Patient ID (or 0 to cancel): ");
        
        if (patientID == 0) {
            cout << "Operation cancelled.\n";
            nextConsultationID--;
            return;
        }
        
        int patientIndex = findPatientIndex(patients, patientID);
        if (patientIndex == -1) {
            cout << "Error: Patient ID does not exist. Please try again or enter 0 to cancel.\n";
            continue;
        }
        
        newConsultation.patientID = patientID;
        cout << "Patient Name: " << patients[patientIndex].name << endl;
        break;
    }

    // Get and validate Doctor ID with retry option
    while (true) {
        displayDoctorList(doctors);
        
        int doctorID = getNonNegativeInt("Enter Doctor ID (or 0 to cancel): ");
        
        if (doctorID == 0) {
            cout << "Operation cancelled.\n";
            nextConsultationID--;
            return;
        }
        
        int doctorIndex = findDoctorIndex(doctors, doctorID);
        if (doctorIndex == -1) {
            cout << "Error: Doctor ID does not exist. Please try again or enter 0 to cancel.\n";
            continue;
        }
        
        newConsultation.doctorID = doctorID;
        newConsultation.doctorType = doctors[doctorIndex].specialization;
        cout << "Doctor: " << doctors[doctorIndex].name << " (" << doctors[doctorIndex].specialization << ")\n";
        break;
    }

    newConsultation.diagnosis = getNonEmptyString("Enter Diagnosis: ");

    // Use validated date from input_utils.h - allows past, present, or future
    newConsultation.consultationDate = getValidatedDate("Enter Consultation Date (DD/MM/YYYY): ");

    // Follow-up date input with proper validation
    string followUpInput;
    do {
        cout << "Enter Follow-up Date (DD/MM/YYYY) or 'None' if not required: ";
        getline(cin, followUpInput);
        
        if (followUpInput.empty()) {
            cout << "[ERROR] Input cannot be empty. Please enter a value or 'None'.\n";
            continue;
        }
        
        // Check if user wants no follow-up
        if (followUpInput == "None" || followUpInput == "none" || followUpInput == "NONE") {
            followUpInput = "None";
            break;
        }
        
        // Validate date format
        if (!isValidDate(followUpInput)) {
            cout << "[ERROR] Invalid date format. Please use DD/MM/YYYY (e.g., 25/12/2026) or 'None'.\n";
            continue;
        }
        // Parse consultation date for comparison
        int consultDay = stoi(newConsultation.consultationDate.substr(0, 2));
        int consultMonth = stoi(newConsultation.consultationDate.substr(3, 2));
        int consultYear = stoi(newConsultation.consultationDate.substr(6, 4));
        
        // Parse follow-up date for comparison
        int followDay = stoi(followUpInput.substr(0, 2));
        int followMonth = stoi(followUpInput.substr(3, 2));
        int followYear = stoi(followUpInput.substr(6, 4));
        
        // Check if follow-up date is after consultation date
        if (followYear < consultYear ||
            (followYear == consultYear && followMonth < consultMonth) ||
            (followYear == consultYear && followMonth == consultMonth && followDay <= consultDay)) {
            cout << "[ERROR] Follow-up date must be after the consultation date (" 
                 << newConsultation.consultationDate << "). Please enter a later date.\n";
            continue;
        }
        break;
    } while (true);
    
    newConsultation.followUpDate = followUpInput;

    consultations.push_back(newConsultation);

    cout << "\n==========================================\n";
    cout << "Consultation recorded successfully.\n";
    cout << "Consultation ID: " << newConsultation.consultationID << endl;
    cout << "Patient ID: " << newConsultation.patientID << endl;
    
    // Display patient name
    int patientIdx = findPatientIndex(patients, newConsultation.patientID);
    if (patientIdx != -1) {
        cout << "Patient Name: " << patients[patientIdx].name << endl;
    }
    
    cout << "Doctor ID: " << newConsultation.doctorID << endl;
    
    // Display doctor name
    int doctorIdx = findDoctorIndex(doctors, newConsultation.doctorID);
    if (doctorIdx != -1) {
        cout << "Doctor Name: " << doctors[doctorIdx].name << endl;
    }
    
    cout << "Diagnosis: " << newConsultation.diagnosis << endl;
    cout << "Date: " << newConsultation.consultationDate << endl;
    cout << "Follow-up: " << newConsultation.followUpDate << endl;
    cout << "==========================================\n";
    
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
    
    int patientID;
    int patientIndex;
    
    // Keep asking until valid patient ID is entered or user cancels
    while (true) {
        patientID = getNonNegativeInt("Enter Patient ID (or 0 to cancel): ");
        
        if (patientID == 0) {
            cout << "Operation cancelled.\n";
            return;
        }
        
        patientIndex = findPatientIndex(patients, patientID);
        
        if (patientIndex == -1) {
            cout << "Error: Patient ID does not exist. Please try again or enter 0 to cancel.\n";
            continue;
        }
        
        // Valid patient found, break out of loop
        break;
    }

    // Display patient information
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
            cout << "Doctor: " << doctorName << endl;
            cout << "Date: " << consultation.consultationDate << endl;
            cout << "Diagnosis: " << consultation.diagnosis << endl;
            
            // Display prescribed medicines
            if (!consultation.prescribedMedicines.empty()) {
                cout << "Medicines Prescribed:\n";
                for (const PrescribedMedicine& pm : consultation.prescribedMedicines) {
                    cout << "- " << pm.medicineName << " x " << pm.quantity << endl;
                }
            } else {
                cout << "Medicines Prescribed: None\n";
            }

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
    vector<Consultation>& consultations,
    vector<Medicine>& medicineInventory,
    const vector<Patient>& patients,
    const vector<Doctor>& doctors
) {
    cout << "\n========== PRESCRIBE MEDICINE ==========\n";

    if (consultations.empty()) {
        cout << "No consultation records available.\n";
        pressZeroToContinue();
        return;
    }

    if (medicineInventory.empty()) {
        cout << "No medicine records available for prescription.\n";
        pressZeroToContinue();
        return;
    }

    // Get and validate Consultation ID
    int consultationID;
    int consultationIndex;
    while (true) {
        consultationID = getNonNegativeInt("Enter Consultation ID (or 0 to cancel): ");
        
        if (consultationID == 0) {
            cout << "Operation cancelled.\n";
            return;
        }
        
        consultationIndex = findConsultationIndex(consultations, consultationID);
        if (consultationIndex == -1) {
            cout << "Error: Consultation ID does not exist. Please try again or enter 0 to cancel.\n";
            continue;
        }
        break;
    }

    Consultation& consultation = consultations[consultationIndex];
    
    // Display consultation details
    int patientIdx = findPatientIndex(patients, consultation.patientID);
    cout << "\nPatient: " << (patientIdx != -1 ? patients[patientIdx].name : "Unknown") << endl;
    
    int doctorIdx = findDoctorIndex(doctors, consultation.doctorID);
    cout << "Doctor: " << (doctorIdx != -1 ? doctors[doctorIdx].name : "Unknown") << endl;
    cout << "Diagnosis: " << consultation.diagnosis << endl;

    // Display medicine list
    displayMedicineList(medicineInventory);

    // Prescribe medicines (allow multiple)
    while (true) {
        cout << "\n--- Add Medicine ---\n" << endl;
        
        // Get and validate Medicine ID with retry option
        int medicineID;
        int medicineIndex;
        while (true) {
            medicineID = getNonNegativeInt("Enter Medicine ID (or 0 to finish): ");
            
            if (medicineID == 0) {
                if (consultation.prescribedMedicines.empty()) {
                    cout << "No medicines prescribed. Operation cancelled.\n";
                    return;
                }
                // Break out of medicine selection
                break;
            }
            
            medicineIndex = findMedicineIndex(medicineInventory, medicineID);
            if (medicineIndex == -1) {
                cout << "Error: Medicine ID does not exist. Please try again or enter 0 to finish.\n";
                continue;
            }
            break;
        }
        
        if (medicineID == 0) {
            break; // User finished adding medicines
        }

        // Get quantity with validation - retry if stock not enough
        int quantity;
        while (true) {
            quantity = getNonNegativeInt("Enter Quantity (or 0 to cancel): ");
            
            if (quantity == 0) {
                cout << "Operation cancelled.\n";
                return;
            }
            
            // Check stock availability
            if (medicineInventory[medicineIndex].quantity < quantity) {
                cout << "Insufficient stock.\n";
                cout << "Available quantity: " << medicineInventory[medicineIndex].quantity << endl;
                cout << "Requested quantity: " << quantity << endl;
                cout << "Please enter a smaller quantity or 0 to cancel.\n";
                continue;
            }
            break;
        }

        // Deduct stock
        medicineInventory[medicineIndex].quantity -= quantity;

        // Add prescribed medicine to consultation
        PrescribedMedicine prescribed;
        prescribed.medicineID = medicineID;
        prescribed.medicineName = medicineInventory[medicineIndex].medicineName;
        prescribed.quantity = quantity;
        consultation.prescribedMedicines.push_back(prescribed);

        cout << "\nMedicine prescribed successfully.\n";
        cout << "Medicine: " << medicineInventory[medicineIndex].medicineName << endl;
        cout << "Quantity: " << quantity << endl;
        cout << "Remaining Stock: " << medicineInventory[medicineIndex].quantity << endl;
        
        // Ask if user wants to prescribe more
        char more = getYesNo("\nPrescribe another medicine? (Y/N): ");
        if (more != 'Y' && more != 'y') {
            break;
        }
    }

    cout << "\n==========================================\n";
    cout << "Prescription completed.\n";
    cout << "Total medicines prescribed: " << consultation.prescribedMedicines.size() << endl;
    cout << "==========================================\n";

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

    cout << "\n========================== MEDICINE LIST ==========================\n";

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
    cout << "\n========== UPDATE MEDICINE STOCK ==========\n";

    // Get and validate Medicine ID with retry option
    int medicineID;
    while (true) {
        medicineID = getNonNegativeInt("Enter Medicine ID (or 0 to cancel): ");
        
        if (medicineID == 0) {
            cout << "Operation cancelled.\n";
            return;
        }
        
        int index = findMedicineIndex(medicineInventory, medicineID);
        if (index == -1) {
            cout << "Medicine not found. Please try again or enter 0 to cancel.\n";
            continue;
        }
        break;
    }

    int index = findMedicineIndex(medicineInventory, medicineID);
    if (index == -1) {
        cout << "Medicine not found.\n";
        pressZeroToContinue();
        return;
    }

    int quantity;
    while (true) {
        quantity = getNonNegativeInt("Enter quantity to add (or 0 to cancel): ");
        
        if (quantity == 0) {
            cout << "Operation cancelled.\n";
            return;
        }
        break;
    }

    medicineInventory[index].quantity += quantity;

    cout << "\n==========================================\n";
    cout << "Medicine stock updated successfully.\n";
    cout << "Medicine ID: " << medicineInventory[index].medicineID << endl;
    cout << "Medicine Name: " << medicineInventory[index].medicineName << endl;
    cout << "New Stock Quantity: " << medicineInventory[index].quantity << endl;
    cout << "==========================================\n";
    
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

    // Use expiry date validation - must be in the future
    newMedicine.expiryDate = getValidatedExpiryDate("Enter Expiry Date (DD/MM/YYYY): ");

    medicineInventory.push_back(newMedicine);

    cout << "\n==========================================\n";
    cout << "New medicine added successfully.\n";
    cout << "Medicine ID: " << newMedicine.medicineID << endl;
    cout << "Medicine Name: " << newMedicine.medicineName << endl;
    cout << "Stock Quantity: " << newMedicine.quantity << endl;
    cout << "Price: RM " << fixed << setprecision(2) << newMedicine.price << endl;
    cout << "Expiry Date: " << newMedicine.expiryDate << endl;
    cout << "==========================================\n";
    
    pressZeroToContinue();
}


// ============================================================
// 8. SEARCH MEDICINE
// ============================================================

void searchMedicine(
    const vector<Medicine>& medicineInventory
) {
    cout << "\n========== SEARCH MEDICINE ==========\n";

    // Get and validate Medicine ID with retry option
    int medicineID;
    while (true) {
        medicineID = getNonNegativeInt("Enter Medicine ID (or 0 to cancel): ");
        
        if (medicineID == 0) {
            cout << "Operation cancelled.\n";
            return;
        }
        
        int index = findMedicineIndex(medicineInventory, medicineID);
        if (index == -1) {
            cout << "Medicine not found. Please try again or enter 0 to cancel.\n";
            continue;
        }
        break;
    }

    int index = findMedicineIndex(medicineInventory, medicineID);
    if (index == -1) {
        cout << "Medicine not found.\n";
        pressZeroToContinue();
        return;
    }

    const Medicine& medicine = medicineInventory[index];

    cout << "\n==========================================\n";
    cout << "Medicine Found\n";
    cout << "Medicine ID: " << medicine.medicineID << endl;
    cout << "Medicine Name: " << medicine.medicineName << endl;
    cout << "Stock Quantity: " << medicine.quantity << endl;
    cout << "Price: RM " << fixed << setprecision(2) << medicine.price << endl;
    cout << "Expiry Date: " << medicine.expiryDate << endl;
    cout << "==========================================\n";

    pressZeroToContinue();
}


// ============================================================
// 9. DELETE MEDICINE
// ============================================================

void deleteMedicine(
    vector<Medicine>& medicineInventory
) {
    cout << "\n========== DELETE MEDICINE ==========\n";

    // Get and validate Medicine ID with retry option
    int medicineID;
    while (true) {
        medicineID = getNonNegativeInt("Enter Medicine ID (or 0 to cancel): ");
        
        if (medicineID == 0) {
            cout << "Operation cancelled.\n";
            return;
        }
        
        int index = findMedicineIndex(medicineInventory, medicineID);
        if (index == -1) {
            cout << "Medicine not found. Please try again or enter 0 to cancel.\n";
            continue;
        }
        break;
    }

    int index = findMedicineIndex(medicineInventory, medicineID);
    if (index == -1) {
        cout << "Medicine not found.\n";
        pressZeroToContinue();
        return;
    }

    cout << "Medicine: " << medicineInventory[index].medicineName << endl;

    char confirm = getYesNo("Are you sure you want to delete this medicine? (Y/N): ");

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
    cout << "\n=============== MEDICINE EXPIRY CHECK ===============\n";

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
    cout << "\n==================== FOLLOW-UP REMINDERS ====================\n";

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
         << setw(17) << "Follow-up Date"
         << setw(12) << "Status"
         << endl;
    cout << string(77, '-') << endl;

    for (const Consultation& consultation : consultations) {
        // Skip if follow-up is "None" or empty
        if (consultation.followUpDate == "None" || consultation.followUpDate.empty()) {
            continue;
        }

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
             << setw(17) << consultation.followUpDate;

        if (isOverdue) {
            cout << "OVERDUE!";
        } else {
            cout << "Pending";
        }
        cout << endl;
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
    vector<Patient>& patients,
    const vector<Doctor>& doctors
) {
    int choice;

    do {
        clearScreen();
        
        setColor(COLOR_DARK_BLUE);
        cout << "\n  ============================================================\n";
        cout << "  |          CONSULTATION & MEDICINE INVENTORY               |\n";
        cout << "  +----------------------------------------------------------+\n";
        cout << "  |                                                          |\n";
        
        setColor(COLOR_LIGHT_BLUE);
        cout << "  |   1.  Record Consultation                                |\n";
        cout << "  |   2.  View Medical History                               |\n";
        cout << "  |   3.  Prescribe Medicine                                 |\n";
        cout << "  |   4.  Display Medicine List                              |\n";
        cout << "  |   5.  Update Medicine Stock                              |\n";
        cout << "  |   6.  Add New Medicine                                   |\n";
        cout << "  |   7.  Search Medicine                                    |\n";
        cout << "  |   8.  Delete Medicine                                    |\n";
        cout << "  |   9.  Check Low Stock Warning                            |\n";
        cout << "  |  10.  Check Medicine Expiry Dates                        |\n";
        cout << "  |  11.  Display Follow-up Reminders                        |\n";
        
        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        
        setColor(COLOR_RED);
        cout << "  |  12.  Back to Main Menu                                  |\n";
        
        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        cout << "  ============================================================\n";
        setColor(COLOR_RESET);

        choice = getValidatedMenuChoice("\n  Enter your choice: ", 1, 12);

        switch (choice) {
            case 1: recordConsultation(consultations, patients); break;
            case 2: viewMedicalHistory(consultations, patients); break;
            case 3: prescribeMedicine(consultations, medicineInventory, patients, doctors); break;
            case 4: displayMedicineList(medicineInventory); pressZeroToContinue(); break;
            case 5: updateMedicineStock(medicineInventory); break;
            case 6: addNewMedicine(medicineInventory); break;
            case 7: searchMedicine(medicineInventory); break;
            case 8: deleteMedicine(medicineInventory); break;
            case 9: displayLowStockWarning(medicineInventory, 10); break;
            case 10: checkExpiryDate(medicineInventory); break;
            case 11: displayFollowUpReminder(consultations, patients); break;
            case 12: cout << "\nReturning to Main Menu...\n"; break;
            default: cout << "\nInvalid choice. Please try again.\n";
        }
    } while (choice != 12);
}