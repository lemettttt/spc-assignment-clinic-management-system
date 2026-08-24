/*
    ============================================================
    shared_data.h
    Common structures and constants used by ALL modules.
    Every member's .cpp file should #include this instead of
    redefining these structs locally, so everyone shares the
    exact same data layout (required for Module Integration &
    Data Sharing).
    ============================================================
*/
#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <string>
#include <vector>
using namespace std;

// ---------------- SHARED CONSTANTS ----------------
const int MAX_LOGIN_ATTEMPTS = 3;

// ---------------- SYSTEM LOGIN (shared team component) ----------------
struct Staff {
    string username;
    string password;
    string role;
};

// ---------------- PATIENT MANAGEMENT (Chia Koh Suan) ----------------
struct Patient {
    int patientID;
    string icNumber;
    string name;
    int age;
    string phone;
    string allergyHistory;
};

// ---------------- APPOINTMENT & QUEUE MANAGEMENT (Pah Yan Han) ----------------
struct Appointment {
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string timeSlot;
    bool isCancelled;
};

// ---------------- CONSULTATION & MEDICINE INVENTORY (Siow Pei En) ----------------
struct Consultation {
    int consultationID;
    int patientID;
    int doctorID;
    string diagnosis;
    string date;
    int followUpDays;   // used for the Automatic Follow-up Reminder extra feature
};

struct Medicine {   // inventory-level record (stock on the shelf)
    int medicineID;
    string name;
    int stockQty;
    double price;
    string expiryDate;
};

// ---------------- BILLING & REPORTING (Soo Hui Xin) ----------------
struct MedicineItem {   // one prescribed line item on a specific bill
    int medicineID;
    string name;
    int qty;
    double price;
};

struct BillRecord {
    int billID;
    int patientID;
    int doctorID;
    string doctorType;
    string serviceDate;
    string diagnosis;
    double consultationFee;
    double medicineFee;
    double discount;
    double sst;
    double totalAmount;
    double amountPaid;
    double changeGiven;
    bool isPaid;
    vector<MedicineItem> medicines;
};

#endif
