/*
    ============================================================
    teammate_placeholders.h
    ============================================================
    PLACEHOLDER ONLY — do not submit this file as-is.

    This exists purely so main.cpp compiles and runs right now,
    before the other 3 members have finished their own modules.
    Each function here should be DELETED once the real teammate
    delivers their own .h/.cpp with a matching function signature:

      void patientManagementMenu(vector<Patient>& patients);
        -> Chia Koh Suan, in patient_management.h / .cpp

      void appointmentQueueMenu(vector<Appointment>& appointments,
                                 vector<Patient>& patients);
        -> Pah Yan Han, in appointment_queue.h / .cpp

      void consultationMedicineMenu(vector<Consultation>& consultations,
                                     vector<Medicine>& medicineInventory,
                                     vector<Patient>& patients);
        -> Siow Pei En, in consultation_medicine.h / .cpp

    When swapping in the real files, just change the #include in
    main.cpp from "teammate_placeholders.h" to their actual header
    files — the function calls in main() do not need to change,
    as long as the signatures above match.
    ============================================================
*/
#ifndef TEAMMATE_PLACEHOLDERS_H
#define TEAMMATE_PLACEHOLDERS_H

#include <iostream>
#include "shared_data.h"
using namespace std;

inline void patientManagementMenu(vector<Patient>& patients) {
    cout << "\n[Placeholder] Patient Management module not yet integrated.\n";
    cout << "Adding a sample patient for testing purposes...\n";
    if (patients.empty()) {
        Patient p;
        p.patientID = 1; p.icNumber = "010101010101"; p.name = "Test Patient";
        p.age = 30; p.phone = "0123456789"; p.allergyHistory = "None";
        patients.push_back(p);
        cout << "Sample patient added (ID: 1).\n";
    }
}

inline void consultationMedicineMenu(vector<Consultation>& consultations,
                                      vector<Medicine>& medicineInventory,
                                      vector<Patient>& patients) {
    cout << "\n[Placeholder] Consultation & Medicine Inventory module not yet integrated.\n";
    cout << "Adding sample consultation and medicine data for testing purposes...\n";
    if (consultations.empty()) {
        Consultation c;
        c.consultationID = 1; c.patientID = 1; c.doctorID = 101;
        c.diagnosis = "Common Cold"; c.date = "24/08/2026"; c.followUpDays = 7;
        consultations.push_back(c);
    }
    if (medicineInventory.empty()) {
        Medicine m1 = {1, "Paracetamol", 100, 2.50, "31/12/2027"};
        Medicine m2 = {2, "Cough Syrup", 50, 8.00, "30/06/2027"};
        medicineInventory.push_back(m1);
        medicineInventory.push_back(m2);
    }
}

#endif
