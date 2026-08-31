#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "clinic_shared.h"
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;

// ============================================================
// Global Shared Data Vectors
// ============================================================

vector<Staff>          staffList;
vector<Patient>        patients;
vector<Appointment>    appointments;
vector<Consultation>   consultations;
vector<Medicine>       medicines;
vector<BillRecord>     billRecords;
vector<Doctor>         doctors;

// ============================================================
// Auto-increment ID Counters
// ============================================================

int nextPatientID      = 1001;
int nextAppointmentID  = 5001;
int nextConsultationID = 7001;
int nextMedicineID     = 3001;

// ============================================================
// Doctor Arrays (used by Appointment module)
// ============================================================

int doctorIDs[MAX_DOCTORS] = {101, 102, 103};
string doctorNames[MAX_DOCTORS] = {"Dr. Ahmad", "Dr. Lim", "Dr. Kumar"};

// ============================================================
// Helper: Get Today's Date (DD/MM/YYYY)
// ============================================================

static string getTodayDateStr() {
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);

    int year  = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day   = localTime->tm_mday;

    ostringstream oss;
    oss << setfill('0') << setw(2) << day << "/"
        << setfill('0') << setw(2) << month << "/"
        << year;

    return oss.str();
}

// ============================================================
// Helper: Get Future Date (N days from today)
// ============================================================

static string getFutureDateStr(int daysAhead) {
    time_t rawTime = time(nullptr);
    rawTime += daysAhead * 24 * 60 * 60;  // Add days in seconds
    tm* localTime = localtime(&rawTime);

    int year  = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day   = localTime->tm_mday;

    ostringstream oss;
    oss << setfill('0') << setw(2) << day << "/"
        << setfill('0') << setw(2) << month << "/"
        << year;

    return oss.str();
}

// ============================================================
// Shared Helper Function Implementations
// ============================================================

int findPatientIndex(const vector<Patient>& patients, int patientID) {
    for (size_t i = 0; i < patients.size(); ++i) {
        if (patients[i].patientID == patientID) return i;
    }
    return -1;
}

int findMedicineIndex(const vector<Medicine>& medicineInventory, int medicineID) {
    for (size_t i = 0; i < medicineInventory.size(); ++i) {
        if (medicineInventory[i].medicineID == medicineID) return i;
    }
    return -1;
}

int findDoctorIndex(const vector<Doctor>& doctors, int doctorID) {
    for (size_t i = 0; i < doctors.size(); ++i) {
        if (doctors[i].doctorID == doctorID) return i;
    }
    return -1;
}

int findConsultationIndex(const vector<Consultation>& consultations, int consultationID) {
    for (size_t i = 0; i < consultations.size(); ++i) {
        if (consultations[i].consultationID == consultationID) return i;
    }
    return -1;
}

int findLatestConsultation(const vector<Consultation>& consultations, int patientID) {
    int latest = -1;
    for (size_t i = 0; i < consultations.size(); ++i) {
        if (consultations[i].patientID == patientID) latest = i;
    }
    return latest;
}

int findConsultationByID(const vector<Consultation>& consultations, int consultationID) {
    for (size_t i = 0; i < consultations.size(); ++i) {
        if (consultations[i].consultationID == consultationID) return i;
    }
    return -1;
}

// ============================================================
// Initialize Doctor Records
// ============================================================

void initDoctorData() {
    if (doctors.empty()) {
        doctors.push_back({101, "Dr. Ahmad", "General"});
        doctors.push_back({102, "Dr. Lim", "Specialist"});
        doctors.push_back({103, "Dr. Kumar", "General"});
    }
}

// ============================================================
// Seed Default Data for Testing
// ============================================================

void seedSharedData() {
    // ---------- Initialize Doctors ----------
    initDoctorData();

    // ---------- Staff Accounts ----------
    staffList.clear();
    staffList.push_back({"admin", "123456", "Admin"});
    staffList.push_back({"reception1", "pass123", "Receptionist"});

    // ---------- Patients (5 patients for testing) ----------
    patients.clear();
    patients.push_back({nextPatientID++, "920101-10-1234", "Tan Wei Ming", 32, 'M', "012-3456789", "Penicillin"});
    patients.push_back({nextPatientID++, "850315-08-5678", "Lim Siew Mei", 40, 'F', "019-8765432", "None"});
    patients.push_back({nextPatientID++, "030505-14-9012", "Ahmad Faiz", 22, 'M', "010-1234567", "Shellfish"});
    patients.push_back({nextPatientID++, "780912-06-3456", "Chong Kar Wai", 47, 'M', "016-5554321", "Aspirin"});
    patients.push_back({nextPatientID++, "950731-12-7890", "Nurul Huda", 30, 'F', "011-9876543", "None"});

    // ---------- Medicines ----------
    medicines.clear();
    medicines.push_back({nextMedicineID++, "Paracetamol 500mg", 50, 0.20, "31/12/2026"});
    medicines.push_back({nextMedicineID++, "Amoxicillin 250mg", 12, 0.55, "15/08/2026"});
    medicines.push_back({nextMedicineID++, "Cough Syrup 100ml",  8, 4.50, "01/09/2026"});
    medicines.push_back({nextMedicineID++, "Loratadine 10mg",   25, 0.30, "30/06/2027"});
    medicines.push_back({nextMedicineID++, "Omeprazole 20mg",   15, 0.80, "29/08/2026"});

    // ---------- Consultations (with prescribed medicines) ----------
    string today = getTodayDateStr();
    string future1 = getFutureDateStr(3);   // 3 days from now
    string future2 = getFutureDateStr(7);   // 7 days from now
    string past1   = getFutureDateStr(-5);  // 5 days ago
    string past2   = getFutureDateStr(-10); // 10 days ago

    consultations.clear();

    // Consultation 1: Patient 1001 (Tan Wei Ming) - today, with medicines
    consultations.push_back({nextConsultationID++, 1001, 101, "General",
                             "Upper Respiratory Infection",
                             today,
                             future1,  // Follow-up in 3 days
                             {{3001, "Paracetamol 500mg", 10},
                              {3002, "Amoxicillin 250mg", 14}}});  // Changed to 3002

    // Consultation 2: Patient 1002 (Lim Siew Mei) - specialist, with medicine
    consultations.push_back({nextConsultationID++, 1002, 102, "Specialist",
                             "Hypertension - Annual Checkup",
                             past1,  // 5 days ago
                             future2,  // Follow-up in 7 days
                             {{3004, "Loratadine 10mg", 5}}});  // Changed to 3004

    // Consultation 3: Patient 1003 (Ahmad Faiz) - past, no follow-up
    consultations.push_back({nextConsultationID++, 1003, 101, "General",
                             "Allergic Reaction",
                             past2,  // 10 days ago
                             "None",
                             {{3001, "Paracetamol 500mg", 5},
                              {3004, "Loratadine 10mg", 10}}});  // Changed to 3004

    // Consultation 4: Patient 1001 (Tan Wei Ming) - older record for history test
    consultations.push_back({nextConsultationID++, 1001, 103, "General",
                             "Routine Checkup",
                             getFutureDateStr(-20),  // 20 days ago
                             "None",
                             {}});

    // Consultation 5: Patient 1004 (Chong Kar Wai) - today, with medicine
    consultations.push_back({nextConsultationID++, 1004, 101, "General",
                             "Gastric Pain",
                             today,
                             getFutureDateStr(14),  // Follow-up in 14 days
                             {{3005, "Omeprazole 20mg", 30}}});  // Changed to 3005

    // ---------- Appointments (today + future) ----------
    appointments.clear();

    // Appointment 1: Patient 1001, Dr. Ahmad, today 09:00 AM
    appointments.push_back({nextAppointmentID++, 1001, 101, "General",
                            today, "09:00 AM", "Pending"});

    // Appointment 2: Patient 1002, Dr. Lim, today 10:00 AM
    appointments.push_back({nextAppointmentID++, 1002, 102, "Specialist",
                            today, "10:00 AM", "Pending"});

    // Appointment 3: Patient 1003, Dr. Ahmad, today 02:00 PM
    appointments.push_back({nextAppointmentID++, 1003, 101, "General",
                            today, "02:00 PM", "Pending"});

    // Appointment 4: Patient 1004, Dr. Kumar, today 03:30 PM (completed)
    appointments.push_back({nextAppointmentID++, 1004, 103, "General",
                            today, "03:30 PM", "Completed"});

    // Appointment 5: Patient 1005, Dr. Lim, future date
    appointments.push_back({nextAppointmentID++, 1005, 102, "Specialist",
                            future1, "09:30 AM", "Pending"});

    // ---------- Bill Records (billing uses generateBill, not manually created) ----------
    // billRecords remains empty; billing module creates bills via generateBill()
    billRecords.clear();
}

// ============================================================
// COLOR CONTROL FUNCTIONS
// ============================================================

void setColor(int color) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    #endif
}