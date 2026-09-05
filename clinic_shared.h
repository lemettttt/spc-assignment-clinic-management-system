#ifndef CLINIC_SHARED_H
#define CLINIC_SHARED_H

// ============================================================
// Shared Structures & Constants
// ============================================================

#include <string>
#include <vector>
using namespace std;

// ============================================================
// Shared Named Constants
// ============================================================
const int    IC_LENGTH             = 14;        // IC format: XXXXXX-XX-XXXX
const int    MAX_DAILY_SLOTS       = 20;        // Max appointment slots per doctor per day
const int    MIN_STOCK_LEVEL       = 10;        // Low stock warning threshold
const int    MIN_PASSWORD_LENGTH   = 6;         // Minimum password length for new accounts
const int    MAX_LOGIN_ATTEMPTS    = 3;         // Max login attempts allowed
const double GENERAL_CONSULT_FEE   = 45.00;     // General consultation fee
const double SPECIALIST_CONSULT_FEE = 80.00;    // Specialist consultation fee
const double SST_RATE              = 0.06;      // Service tax rate 6%
const int    MAX_DOCTORS           = 3;         // Maximum number of doctors

// Color constants
const int COLOR_RESET        = 7;    // White (default)
const int COLOR_DARK_BLUE    = 1;    // Dark blue
const int COLOR_BRIGHT_BLUE  = 9;    // Bright blue
const int COLOR_CYAN         = 3;    // Cyan
const int COLOR_LIGHT_BLUE   = 11;   // Light blue
const int COLOR_RED          = 12;   // Bright red

// Function to set console color
void setColor(int color);

// ============================================================
// STRUCT 0: Staff
// ============================================================
struct Staff {
    string username;
    string password;
    string role;
};

// ============================================================
// STRUCT 1: Patient (Patient Management)
// ============================================================
struct Patient {
    int patientID;
    string icNumber;
    string name;
    int age;
    char gender;
    string phone;
    string allergyHistory;
};

// ============================================================
// STRUCT 2: Appointment
// ============================================================
struct Appointment {
    int appointmentID;
    int patientID;
    int doctorID;
    string doctorType;
    string appointmentDate;
    string timeSlot;
    string status;
};

// ============================================================
// STRUCT 3: PrescribedMedicine
// ============================================================
struct PrescribedMedicine {
    int medicineID;
    string medicineName;
    int quantity;
};

// ============================================================
// STRUCT 4: Consultation
// ============================================================
struct Consultation {
    int consultationID;
    int patientID;
    int doctorID;
    string doctorType;
    string diagnosis;
    string consultationDate;
    string followUpDate;
    vector<PrescribedMedicine> prescribedMedicines;
};

// ============================================================
// STRUCT 5: Medicine (maintained by Consultation module)
// ============================================================
struct Medicine {
    int medicineID;
    string medicineName;
    int quantity;
    double price;
    string expiryDate;
};

// ============================================================
// STRUCT 6: MedicineItem (used in billing)
// ============================================================
struct MedicineItem {
    string name;
    int qty;
    double price;
};

// ============================================================
// STRUCT 7: BillRecord (maintained by Billing module)
// ============================================================
struct BillRecord {
    int billID;
    int patientID;
    int doctorID;
    int consultationID;
    string serviceDate;
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

// ============================================================
// STRUCT 8: Doctor
// ============================================================
struct Doctor {
    int doctorID;
    string name;
    string specialization;
};

// ============================================================
// Global Shared Data
// ============================================================
extern vector<Staff>          staffList;
extern vector<Patient>        patients;
extern vector<Appointment>    appointments;
extern vector<Consultation>   consultations;
extern vector<Medicine>       medicines;
extern vector<BillRecord>     billRecords;
extern vector<Doctor>         doctors;

extern int doctorIDs[MAX_DOCTORS];
extern string doctorNames[MAX_DOCTORS];

extern int nextPatientID;
extern int nextAppointmentID;
extern int nextConsultationID;
extern int nextMedicineID;

void seedSharedData();
void initDoctorData();

// ============================================================
// Shared Helper Functions (for finding indices)
// ============================================================

int findPatientIndex(const vector<Patient>& patients, int patientID);
int findMedicineIndex(const vector<Medicine>& medicineInventory, int medicineID);
int findDoctorIndex(const vector<Doctor>& doctors, int doctorID);
int findConsultationIndex(const vector<Consultation>& consultations, int consultationID);
int findLatestConsultation(const vector<Consultation>& consultations, int patientID);
int findConsultationByID(const vector<Consultation>& consultations, int consultationID);

#endif