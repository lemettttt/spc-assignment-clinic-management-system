#include "clinic_shared.h"
#include <fstream>
#include <sstream>
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

// Doctor arrays for backward compatibility with appointment module
int doctorIDs[MAX_DOCTORS] = {101, 102, 103};
string doctorNames[MAX_DOCTORS] = {"Dr. Ahmad", "Dr. Lim", "Dr. Kumar"};

// ============================================================
// Initialize doctor data
// ============================================================
void initDoctorData() {
    // Only initialize if empty
    if (doctors.empty()) {
        doctors.push_back({101, "Dr. Ahmad", "General"});
        doctors.push_back({102, "Dr. Lim", "Specialist"});
        doctors.push_back({103, "Dr. Kumar", "General"});
    }
}

// ============================================================
// 保存员工数据到文件 staff.txt
// 格式：每行一个账号，用逗号分隔：用户名,密码,角色
// ============================================================
void saveStaffData() {
    ofstream outFile("staff.txt");
    if (!outFile) {
        return;
    }

    for (int i = 0; i < (int)staffList.size(); i++) {
        outFile << staffList[i].username << ","
                << staffList[i].password << ","
                << staffList[i].role << "\n";
    }
    outFile.close();
}

// ============================================================
// 从文件 staff.txt 加载员工数据
// ============================================================
void loadStaffData() {
    ifstream inFile("staff.txt");
    if (!inFile) {
        return;
    }

    staffList.clear();

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string username, password, role;

        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, role, ',');

        if (!username.empty()) {
            staffList.push_back({username, password, role});
        }
    }
    inFile.close();
}

// ============================================================
// seedSharedData() 目的：程序一启动就准备资料
// ============================================================
void seedSharedData() {
    initDoctorData();
    // 先尝试从 staff.txt 加载已有账号
    loadStaffData();

    // 如果加载成功，就不用添加默认账号了
    if (!staffList.empty()) {
        // 药物库存示范资料（如果还没添加过）
        if (medicines.empty()) {
            medicines.push_back({nextMedicineID++, "Paracetamol 500mg", 50, 0.20, "31/12/2026"});
            medicines.push_back({nextMedicineID++, "Amoxicillin 250mg", 12, 0.55, "15/10/2026"});
            medicines.push_back({nextMedicineID++, "Cough Syrup 100ml",  8, 4.50, "01/09/2026"});
        }
        return;
    }

    // ============================================================
    // 如果是第一次运行（staff.txt 不存在），创建默认账号
    // ============================================================
    staffList.push_back({"admin", "1234", "Admin"});
    staffList.push_back({"reception1", "pass123", "Receptionist"});

    // 立即保存到文件，这样下次启动就能读到了
    saveStaffData();

    // 药物库存示范资料
    medicines.push_back({nextMedicineID++, "Paracetamol 500mg", 50, 0.20, "31/12/2026"});
    medicines.push_back({nextMedicineID++, "Amoxicillin 250mg", 12, 0.55, "15/10/2026"});
    medicines.push_back({nextMedicineID++, "Cough Syrup 100ml",  8, 4.50, "01/09/2026"});
}