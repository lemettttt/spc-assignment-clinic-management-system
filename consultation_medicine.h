#ifndef CONSULTATION_MEDICINE_H
#define CONSULTATION_MEDICINE_H

#include <vector>
#include <string>
#include "clinic_shared.h"

using namespace std;

// NOTE: PrescribedMedicine and Consultation are defined in clinic_shared.h.
// Do not redefine them here.

// ============================================================
// Function Prototypes
// ============================================================

void consultationMedicineMenu(
    vector<Consultation>& consultations,
    vector<Medicine>& medicineInventory,
    vector<Patient>& patients,
    const vector<Doctor>& doctors
);

void recordConsultation(
    vector<Consultation>& consultations,
    const vector<Patient>& patients
);

void viewMedicalHistory(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
);

void prescribeMedicine(
    vector<Consultation>& consultations,
    vector<Medicine>& medicineInventory,
    const vector<Patient>& patients,
    const vector<Doctor>& doctors
);

void displayMedicineList(
    const vector<Medicine>& medicineInventory
);

void updateMedicineStock(
    vector<Medicine>& medicineInventory
);

void addNewMedicine(
    vector<Medicine>& medicineInventory
);

void searchMedicine(
    const vector<Medicine>& medicineInventory
);

void deleteMedicine(
    vector<Medicine>& medicineInventory
);

void displayLowStockWarning(
    const vector<Medicine>& medicineInventory,
    int threshold
);

void checkExpiryDate(
    const vector<Medicine>& medicineInventory
);

void displayFollowUpReminder(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
);

void displayDoctorList(const vector<Doctor>& doctors);

bool checkStockAvailability(
    const vector<Medicine>& medicineInventory,
    int medicineID,
    int quantity
);

#endif