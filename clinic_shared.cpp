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
    rawTime += daysAhead * 24 * 60 * 60;
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
    initDoctorData();

    staffList.clear();
    staffList.push_back({"admin", "123456", "Admin"});
    staffList.push_back({"reception1", "pass123", "Receptionist"});

    patients.clear();
    patients.push_back({nextPatientID++, "920101-10-1234", "Tan Wei Ming", 32, 'M', "012-3456789", "Penicillin"});
    patients.push_back({nextPatientID++, "850315-08-5678", "Lim Siew Mei", 40, 'F', "019-8765432", "None"});
    patients.push_back({nextPatientID++, "030505-14-9012", "Ahmad Faiz", 22, 'M', "010-1234567", "Shellfish"});
    patients.push_back({nextPatientID++, "780912-06-3456", "Chong Kar Wai", 47, 'M', "016-5554321", "Aspirin"});
    patients.push_back({nextPatientID++, "950731-12-7890", "Nurul Huda", 30, 'F', "011-9876543", "None"});

    medicines.clear();
    medicines.push_back({nextMedicineID++, "Paracetamol 500mg", 50, 0.20, "31/12/2026"});
    medicines.push_back({nextMedicineID++, "Amoxicillin 250mg", 12, 0.55, "15/10/2026"});
    medicines.push_back({nextMedicineID++, "Cough Syrup 100ml",  8, 4.50, "01/09/2026"});
    medicines.push_back({nextMedicineID++, "Loratadine 10mg",   25, 0.30, "30/06/2027"});
    medicines.push_back({nextMedicineID++, "Omeprazole 20mg",   15, 0.80, "15/12/2026"});

    string today = getTodayDateStr();
    string future1 = getFutureDateStr(3);
    string future2 = getFutureDateStr(7);
    string past1   = getFutureDateStr(-5);
    string past2   = getFutureDateStr(-10);

    consultations.clear();
    consultations.push_back({nextConsultationID++, 1001, 101, "General",
                             "Upper Respiratory Infection", today, future1,
                             {{3001, "Paracetamol 500mg", 10}, {3002, "Amoxicillin 250mg", 14}}});
    consultations.push_back({nextConsultationID++, 1002, 102, "Specialist",
                             "Hypertension - Annual Checkup", past1, future2,
                             {{3004, "Loratadine 10mg", 5}}});
    consultations.push_back({nextConsultationID++, 1003, 101, "General",
                             "Allergic Reaction", past2, "None",
                             {{3001, "Paracetamol 500mg", 5}, {3004, "Loratadine 10mg", 10}}});
    consultations.push_back({nextConsultationID++, 1001, 103, "General",
                             "Routine Checkup", getFutureDateStr(-20), "None", {}});
    consultations.push_back({nextConsultationID++, 1004, 101, "General",
                             "Gastric Pain", today, getFutureDateStr(14),
                             {{3005, "Omeprazole 20mg", 30}}});

    appointments.clear();
    appointments.push_back({nextAppointmentID++, 1001, 101, "General", today, "09:00 AM", "Pending"});
    appointments.push_back({nextAppointmentID++, 1002, 102, "Specialist", today, "10:00 AM", "Pending"});
    appointments.push_back({nextAppointmentID++, 1003, 101, "General", today, "02:00 PM", "Pending"});
    appointments.push_back({nextAppointmentID++, 1004, 103, "General", today, "03:30 PM", "Completed"});
    appointments.push_back({nextAppointmentID++, 1005, 102, "Specialist", future1, "09:30 AM", "Pending"});

    billRecords.clear();
}

// ============================================================
// Shared Helper Function Implementations
// ============================================================

int findPatientIndex(const vector<Patient>& patients, int patientID) {
    for (size_t i = 0; i < patients.size(); ++i) {
        if (patients[i].patientID == patientID) return (int)i;
    }
    return -1;
}

int findMedicineIndex(const vector<Medicine>& medicineInventory, int medicineID) {
    for (size_t i = 0; i < medicineInventory.size(); ++i) {
        if (medicineInventory[i].medicineID == medicineID) return (int)i;
    }
    return -1;
}

int findDoctorIndex(const vector<Doctor>& doctors, int doctorID) {
    for (size_t i = 0; i < doctors.size(); ++i) {
        if (doctors[i].doctorID == doctorID) return (int)i;
    }
    return -1;
}

int findConsultationIndex(const vector<Consultation>& consultations, int consultationID) {
    for (size_t i = 0; i < consultations.size(); ++i) {
        if (consultations[i].consultationID == consultationID) return (int)i;
    }
    return -1;
}

int findLatestConsultation(const vector<Consultation>& consultations, int patientID) {
    int latest = -1;
    for (size_t i = 0; i < consultations.size(); ++i) {
        if (consultations[i].patientID == patientID) latest = (int)i;
    }
    return latest;
}

int findConsultationByID(const vector<Consultation>& consultations, int consultationID) {
    for (size_t i = 0; i < consultations.size(); ++i) {
        if (consultations[i].consultationID == consultationID) return (int)i;
    }
    return -1;
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