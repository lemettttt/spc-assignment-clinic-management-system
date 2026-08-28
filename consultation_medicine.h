#ifndef CONSULTATION_MEDICINE_H
#define CONSULTATION_MEDICINE_H

#include "clinic_shared.h"
#include <vector>
#include <string>
using namespace std;

int findPatientIndex(const vector<Patient>& patients, int patientID);
int findMedicineIndex(const vector<Medicine>& medicineInventory, int medicineID);
int findConsultationIndex(const vector<Consultation>& consultations, int consultationID);
int findDoctorIndex(const vector<Doctor>& doctors, int doctorID);  
void displayDoctorList(const vector<Doctor>& doctors);            

// Main menu
void consultationMedicineMenu(
    vector<Consultation>& consultations,
    vector<Medicine>& medicineInventory,
    vector<Patient>& patients
);

// Consultation functions
void recordConsultation(
    vector<Consultation>& consultations,
    const vector<Patient>& patients
);

void viewMedicalHistory(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
);

// Medicine functions
void prescribeMedicine(
    vector<Medicine>& medicineInventory
);

bool checkStockAvailability(
    const vector<Medicine>& medicineInventory,
    int medicineID,
    int quantity
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

// Extra functions
void checkExpiryDate(
    const vector<Medicine>& medicineInventory
);

void displayFollowUpReminder(
    const vector<Consultation>& consultations,
    const vector<Patient>& patients
);

void displayLowStockWarning(
    const vector<Medicine>& medicineInventory,
    int threshold
);

#endif