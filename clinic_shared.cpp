
#include "clinic_shared.h"

vector<Staff>          staffList;
vector<Patient>        patients;
vector<Appointment>    appointments;
vector<Consultation>   consultations;
vector<Medicine>       medicines;
vector<BillRecord>     billRecords;

int nextPatientID      = 1001;
int nextAppointmentID  = 5001;
int nextConsultationID = 7001;
int nextMedicineID     = 3001;

// ============================================================
// seedSharedData() 目的：程序一启动就放几笔示范资料进去，方便大家测试。
// ============================================================
void seedSharedData() {
    // 员工登入帐号（跟hx原本main()里的dummy data一样）
    staffList.push_back({"admin", "1234", "Admin"});
    staffList.push_back({"reception1", "pass123", "Receptionist"});

    // 药物库存示范资料（给Consultation & Medicine模块测试用）
    medicines.push_back({nextMedicineID++, "Paracetamol 500mg", 50, 0.20, "31/12/2026"});
    medicines.push_back({nextMedicineID++, "Amoxicillin 250mg", 12, 0.55, "15/10/2026"});
    medicines.push_back({nextMedicineID++, "Cough Syrup 100ml",  8, 4.50, "01/09/2026"});
}
