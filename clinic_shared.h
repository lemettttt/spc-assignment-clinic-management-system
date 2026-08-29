#ifndef CLINIC_SHARED_H
#define CLINIC_SHARED_H

//============================================================
//  Shared Structures & Constants
//============================================================

#include <string>
#include <vector>
using namespace std;

// ============================================================
// Shared Named Constants
// ============================================================
const int    IC_LENGTH             = 14;        // IC format (format: XXXXXX-XX-XXXX)
const int    MAX_DAILY_SLOTS       = 20;        // 每位医生每天最多可预约的时段数
const int    MIN_STOCK_LEVEL       = 10;        // 药物低库存警示门槛
const int    MIN_PASSWORD_LENGTH   = 6;         // 注册新帐号时密码最短长度
const int    MAX_LOGIN_ATTEMPTS    = 3;         // 登入最多允许失败几次（来自Lemet）
const double GENERAL_CONSULT_FEE   = 45.00;     // 普通科诊金（来自Lemet）
const double SPECIALIST_CONSULT_FEE = 80.00;    // 专科诊金（来自Lemet，取代原本的PEDIATRIC_CONSULT_FEE）
const double SST_RATE              = 0.06;      // 服务税率6%（来自Lemet）
const int MAX_DOCTORS = 3;

// ============================================================
// STRUCT 0：Staff
// ------------------------------------------------------------
struct Staff {
    string username;
    string password;
    string role;    // Eg: "Nurse", "Doctor"
};

// ============================================================
// STRUCT 1：Patient —— Patient Management
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
// STRUCT 2：Appointment 
// ------------------------------------------------------------
struct Appointment {
    int appointmentID;
    int patientID;
    int doctorID;
    string doctorType;      // "General" or "Specialist"
    string appointmentDate;
    string timeSlot;
    string status;           // "Pending" / "Completed" / "Cancelled"
};

// ============================================================
// STRUCT 3: PrescribedMedicine (Must be defined before Consultation)
// ============================================================
struct PrescribedMedicine {
    int medicineID;
    string medicineName;
    int quantity;
};

// ============================================================
// STRUCT 4：Consultation
// ============================================================
struct Consultation {
    int consultationID;
    int patientID;
    int doctorID;
    string doctorType;
    string diagnosis;
    string consultationDate;
    string followUpDate;
    vector<PrescribedMedicine> prescribedMedicines;  // List of prescribed medicines
};

// ============================================================
// STRUCT 5：Medicine —— Consultation模块负责人维护
// ------------------------------------------------------------
struct Medicine {
    int medicineID;
    string medicineName;
    int quantity;
    double price;
    string expiryDate;
};

// ============================================================
// STRUCT 6：MedicineItem 
// ------------------------------------------------------------
struct MedicineItem {
    string name;
    int qty;
    double price;
};

// ============================================================
// STRUCT 7：BillRecord —— Billing模块负责人维护
// ------------------------------------------------------------
struct BillRecord {
    int billID;
    int patientID;
    int doctorID;
    string doctorType;       // "General" or "Specialist"
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
void saveStaffData();
void loadStaffData();
void initDoctorData();

#endif
