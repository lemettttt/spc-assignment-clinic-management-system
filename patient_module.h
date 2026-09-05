#ifndef PATIENT_MODULE_H
#define PATIENT_MODULE_H

#include "clinic_shared.h"

// ============================================================
// Function Prototypes
// ============================================================

bool validateICFormat(const string& ic);
bool equalsIgnoreCase(const string& a, const string& b);
bool checkDuplicateIC(const string& ic);

int searchPatientByIC(const string& ic);
int searchPatientByName(const string& name);

void registerNewPatient();
void updatePatientInfo();
void displayPatientRecord(const Patient& p);
void viewAllPatients();

void patientManagementMenu();

#endif