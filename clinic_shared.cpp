// 先定义宏，再包含 windows.h，避免 byte 冲突
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "clinic_shared.h"
using namespace std;

vector<Staff>          staffList;
vector<Patient>        patients;
vector<Appointment>    appointments;
vector<Consultation>   consultations;
vector<Medicine>       medicines;
vector<BillRecord>     billRecords;
vector<Doctor>         doctors;

int nextPatientID      = 1001;
int nextAppointmentID  = 5001;
int nextConsultationID = 7001;
int nextMedicineID     = 3001;

int doctorIDs[MAX_DOCTORS] = {101, 102, 103};
string doctorNames[MAX_DOCTORS] = {"Dr. Ahmad", "Dr. Lim", "Dr. Kumar"};

void initDoctorData() {
    if (doctors.empty()) {
        doctors.push_back({101, "Dr. Ahmad", "General"});
        doctors.push_back({102, "Dr. Lim", "Specialist"});
        doctors.push_back({103, "Dr. Kumar", "General"});
    }
}

void seedSharedData() {
    initDoctorData();
    staffList.clear();
    staffList.push_back({"admin", "1234", "Admin"});
    staffList.push_back({"reception1", "pass123", "Receptionist"});
    medicines.clear();
    medicines.push_back({nextMedicineID++, "Paracetamol 500mg", 50, 0.20, "31/12/2026"});
    medicines.push_back({nextMedicineID++, "Amoxicillin 250mg", 12, 0.55, "15/10/2026"});
    medicines.push_back({nextMedicineID++, "Cough Syrup 100ml",  8, 4.50, "01/09/2026"});
}

// ============================================================
// 颜色控制函数实现
// ============================================================

void setColor(int color) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    #endif
}