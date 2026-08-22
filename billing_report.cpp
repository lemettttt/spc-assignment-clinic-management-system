/*
    ============================================================
    Clinic Management System
    Module: Login Management + Billing & Reporting
    Author: Lemet
    ============================================================
    This file demonstrates the assigned module ONLY (standalone).
    When integrated with teammates' modules:
      - staffList would come from a shared login/staff module
      - patientID / doctorID would be validated against
        Patient Management and Appointment modules
      - medicines/prices could come from the Consultation &
        Medicine Inventory module
    For now, dummy/sample data is used so this module can be
    compiled and tested independently.
    ============================================================
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

// ---------------- CONSTANTS ----------------
const double GENERAL_CONSULT_FEE    = 45.00;
const double SPECIALIST_CONSULT_FEE = 80.00;
const double SST_RATE               = 0.06;
const int    MAX_LOGIN_ATTEMPTS     = 3;

// ---------------- STRUCTURES ----------------
struct Staff {
    string username;
    string password;
    string role;
};

struct MedicineItem {
    string name;
    int qty;
    double price;
};

struct BillRecord {
    int billID;
    int patientID;
    int doctorID;
    string doctorType;      // "General" or "Specialist"
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

// ---------------- FUNCTION PROTOTYPES ----------------
bool login(vector<Staff> staffList, string username, string password);

double calculateConsultationFee(string doctorType);
double calculateMedicineFee(vector<MedicineItem> medicines);
void   addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill);
void   generateBill(vector<BillRecord>& billRecords, int patientID, int doctorID,
                     string doctorType, string date, vector<MedicineItem> medicines, double discount);
int    searchBillingRecord(vector<BillRecord> billRecords, int billID);
bool   processPayment(BillRecord& bill, double amountReceived, char paymentMethod);
void   printReceipt(BillRecord bill);
void   generateDailyReport(vector<BillRecord> billRecords, string date);
void   generateStatisticsReport(vector<BillRecord> billRecords, int reportOption);
double getValidatedPayment(double minAmount);
void   billingReportMenu(vector<BillRecord>& billRecords);

// ============================================================
// LOGIN MANAGEMENT
// ============================================================
bool login(vector<Staff> staffList, string username, string password) {
    // linear search through staff list (vector) to validate credentials
    for (int i = 0; i < (int)staffList.size(); i++) {
        if (staffList[i].username == username && staffList[i].password == password) {
            return true;
        }
    }
    return false;
}

// ============================================================
// BILLING FUNCTIONS
// ============================================================
double calculateConsultationFee(string doctorType) {
    // selection: if-else
    if (doctorType == "Specialist") {
        return SPECIALIST_CONSULT_FEE;
    } else {
        return GENERAL_CONSULT_FEE;
    }
}

double calculateMedicineFee(vector<MedicineItem> medicines) {
    double total = 0;
    // repetition: for loop over vector
    for (int i = 0; i < (int)medicines.size(); i++) {
        total += medicines[i].qty * medicines[i].price;
    }
    return total;
}

void addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill) {
    // pass-by-reference: directly modifies the shared vector
    billRecords.push_back(newBill);
    cout << "\nBill generated successfully. Bill ID: " << newBill.billID << endl;
}

void generateBill(vector<BillRecord>& billRecords, int patientID, int doctorID,
                   string doctorType, string date, vector<MedicineItem> medicines, double discount) {
    BillRecord newBill;
    newBill.billID        = 1000 + (int)billRecords.size() + 1;
    newBill.patientID     = patientID;
    newBill.doctorID      = doctorID;
    newBill.doctorType    = doctorType;
    newBill.serviceDate   = date;
    newBill.medicines     = medicines;

    // return values from calculateConsultationFee() and calculateMedicineFee()
    // are combined here to produce the final total
    newBill.consultationFee = calculateConsultationFee(doctorType);
    newBill.medicineFee     = calculateMedicineFee(medicines);
    newBill.discount        = discount;

    double subtotal = newBill.consultationFee + newBill.medicineFee - discount;
    newBill.sst          = subtotal * SST_RATE;
    newBill.totalAmount  = subtotal + newBill.sst;
    newBill.amountPaid   = 0;
    newBill.changeGiven  = 0;
    newBill.isPaid       = false;

    addBillingRecord(billRecords, newBill);
}

int searchBillingRecord(vector<BillRecord> billRecords, int billID) {
    for (int i = 0; i < (int)billRecords.size(); i++) {
        if (billRecords[i].billID == billID) {
            return i;   // found - return index
        }
    }
    return -1;          // not found
}

double getValidatedPayment(double minAmount) {
    double amount;
    // repetition: while loop for input validation
    while (true) {
        cout << "Enter payment amount (RM): ";
        cin >> amount;
        if (cin.fail() || amount < 0) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a positive number.\n";
        } else {
            return amount;
        }
    }
}

bool processPayment(BillRecord& bill, double amountReceived, char paymentMethod) {
    // nested if: outer checks payment method, inner checks sufficiency
    if (paymentMethod == 'C' || paymentMethod == 'c') {
        if (amountReceived >= bill.totalAmount) {
            bill.amountPaid  = amountReceived;
            bill.changeGiven = amountReceived - bill.totalAmount;
            bill.isPaid      = true;
            cout << fixed << setprecision(2);
            cout << "Payment successful. Change: RM " << bill.changeGiven << endl;
            return true;
        } else {
            cout << "Insufficient payment! Amount short by RM "
                 << fixed << setprecision(2) << (bill.totalAmount - amountReceived) << endl;
            return false;
        }
    } else if (paymentMethod == 'D' || paymentMethod == 'd') {
        bill.amountPaid  = bill.totalAmount;
        bill.changeGiven = 0;
        bill.isPaid      = true;
        cout << "Payment approved via debit/card." << endl;
        return true;
    } else {
        cout << "Invalid payment method!" << endl;
        return false;
    }
}

void printReceipt(BillRecord bill) {
    cout << fixed << setprecision(2);
    cout << "\n========================================\n";
    cout << "              CLINIC RECEIPT             \n";
    cout << "========================================\n";
    cout << "Bill ID      : " << bill.billID << endl;
    cout << "Patient ID   : " << bill.patientID << endl;
    cout << "Date         : " << bill.serviceDate << endl;
    cout << "----------------------------------------\n";
    cout << setw(22) << left << "Consultation Fee" << "RM " << setw(8) << right << bill.consultationFee << endl;
    cout << setw(22) << left << "Medicine Fee"      << "RM " << setw(8) << right << bill.medicineFee << endl;
    cout << setw(22) << left << "Discount"          << "RM " << setw(8) << right << bill.discount << endl;
    cout << setw(22) << left << "SST (6%)"          << "RM " << setw(8) << right << bill.sst << endl;
    cout << "----------------------------------------\n";
    cout << setw(22) << left << "Total Payable"     << "RM " << setw(8) << right << bill.totalAmount << endl;
    cout << setw(22) << left << "Amount Paid"       << "RM " << setw(8) << right << bill.amountPaid << endl;
    cout << setw(22) << left << "Change"            << "RM " << setw(8) << right << bill.changeGiven << endl;
    cout << "========================================\n";
    cout << (bill.isPaid ? "Status: PAID" : "Status: UNPAID") << "\n\n";
}

// ============================================================
// REPORTING FUNCTIONS
// ============================================================
void generateDailyReport(vector<BillRecord> billRecords, string date) {
    double totalRevenue = 0;
    int count = 0;

    cout << fixed << setprecision(2);
    cout << "\n========================================\n";
    cout << "        DAILY REVENUE REPORT (" << date << ")\n";
    cout << "========================================\n";
    cout << setw(10) << left << "Bill ID" << setw(15) << "Patient ID" << "Total (RM)" << endl;
    cout << "----------------------------------------\n";

    for (int i = 0; i < (int)billRecords.size(); i++) {
        if (billRecords[i].serviceDate == date) {
            cout << setw(10) << left << billRecords[i].billID
                 << setw(15) << billRecords[i].patientID
                 << billRecords[i].totalAmount << endl;
            totalRevenue += billRecords[i].totalAmount;
            count++;
        }
    }
    cout << "----------------------------------------\n";
    cout << "Total Patients Billed : " << count << endl;
    cout << "Total Revenue         : RM " << totalRevenue << endl;
    cout << "========================================\n";
}

void generateStatisticsReport(vector<BillRecord> billRecords, int reportOption) {
    cout << fixed << setprecision(2);

    // switch: one function covers multiple management report types
    switch (reportOption) {
        case 1: {  // Doctor workload
            cout << "\n----- DOCTOR WORKLOAD REPORT -----\n";
            vector<int> doctorIDs;
            vector<int> visitCount;
            for (int i = 0; i < (int)billRecords.size(); i++) {
                int id = billRecords[i].doctorID;
                int idx = -1;
                for (int j = 0; j < (int)doctorIDs.size(); j++) {
                    if (doctorIDs[j] == id) { idx = j; break; }
                }
                if (idx == -1) {
                    doctorIDs.push_back(id);
                    visitCount.push_back(1);
                } else {
                    visitCount[idx]++;
                }
            }
            for (int i = 0; i < (int)doctorIDs.size(); i++) {
                cout << "Doctor ID " << doctorIDs[i] << " : " << visitCount[i] << " patients seen\n";
            }
            break;
        }
        case 2: {  // Revenue summary
            double consultTotal = 0, medTotal = 0, sstTotal = 0, grandTotal = 0;
            for (int i = 0; i < (int)billRecords.size(); i++) {
                consultTotal += billRecords[i].consultationFee;
                medTotal     += billRecords[i].medicineFee;
                sstTotal     += billRecords[i].sst;
                grandTotal   += billRecords[i].totalAmount;
            }
            cout << "\n----- REVENUE SUMMARY REPORT -----\n";
            cout << "Consultation Fees : RM " << consultTotal << endl;
            cout << "Medicine Sales    : RM " << medTotal << endl;
            cout << "SST Collected     : RM " << sstTotal << endl;
            cout << "Total Revenue     : RM " << grandTotal << endl;
            break;
        }
        case 3: {  // Medicine usage
            cout << "\n----- MEDICINE USAGE REPORT -----\n";
            vector<string> names;
            vector<int> quantities;
            for (int i = 0; i < (int)billRecords.size(); i++) {
                for (int j = 0; j < (int)billRecords[i].medicines.size(); j++) {
                    string name = billRecords[i].medicines[j].name;
                    int qty = billRecords[i].medicines[j].qty;
                    int idx = -1;
                    for (int k = 0; k < (int)names.size(); k++) {
                        if (names[k] == name) { idx = k; break; }
                    }
                    if (idx == -1) {
                        names.push_back(name);
                        quantities.push_back(qty);
                    } else {
                        quantities[idx] += qty;
                    }
                }
            }
            for (int i = 0; i < (int)names.size(); i++) {
                cout << names[i] << " : " << quantities[i] << " units sold\n";
            }
            break;
        }
        default:
            cout << "Invalid report option!\n";
    }
    cout << "========================================\n";
}

// ============================================================
// MENU (do-while + switch)
// ============================================================
void billingReportMenu(vector<BillRecord>& billRecords) {
    int choice;
    do {
        cout << "\n========== BILLING & REPORT MENU ==========\n";
        cout << "1. Generate Bill\n";
        cout << "2. Process Payment\n";
        cout << "3. Search Bill\n";
        cout << "4. Print Receipt\n";
        cout << "5. Generate Daily Report\n";
        cout << "6. Generate Management/Statistics Report\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int patientID, doctorID;
                string doctorType, date;
                cout << "Patient ID: "; cin >> patientID;
                cout << "Doctor ID: "; cin >> doctorID;
                cout << "Doctor Type (General/Specialist): "; cin >> doctorType;
                cout << "Date (DD/MM/YYYY): "; cin >> date;

                vector<MedicineItem> medicines;
                int medCount;
                cout << "Number of medicine items: "; cin >> medCount;
                for (int i = 0; i < medCount; i++) {
                    MedicineItem m;
                    cout << "  Medicine name: "; cin >> m.name;
                    cout << "  Quantity: "; cin >> m.qty;
                    cout << "  Unit price: "; cin >> m.price;
                    medicines.push_back(m);
                }
                double discount;
                cout << "Discount (RM, 0 if none): "; cin >> discount;

                generateBill(billRecords, patientID, doctorID, doctorType, date, medicines, discount);
                break;
            }
            case 2: {
                int billID;
                cout << "Enter Bill ID: "; cin >> billID;
                int idx = searchBillingRecord(billRecords, billID);
                if (idx == -1) {
                    cout << "Bill not found!\n";
                } else {
                    double amount = getValidatedPayment(0);
                    char method;
                    cout << "Payment method (C=Cash, D=Debit): "; cin >> method;
                    processPayment(billRecords[idx], amount, method);
                }
                break;
            }
            case 3: {
                int billID;
                cout << "Enter Bill ID: "; cin >> billID;
                int idx = searchBillingRecord(billRecords, billID);
                cout << (idx == -1 ? "Bill not found!\n" : "Bill found at record.\n");
                break;
            }
            case 4: {
                int billID;
                cout << "Enter Bill ID: "; cin >> billID;
                int idx = searchBillingRecord(billRecords, billID);
                if (idx == -1) cout << "Bill not found!\n";
                else printReceipt(billRecords[idx]);
                break;
            }
            case 5: {
                string date;
                cout << "Enter date (DD/MM/YYYY): "; cin >> date;
                generateDailyReport(billRecords, date);
                break;
            }
            case 6: {
                int option;
                cout << "1. Doctor Workload  2. Revenue Summary  3. Medicine Usage\n";
                cout << "Choice: "; cin >> option;
                generateStatisticsReport(billRecords, option);
                break;
            }
            case 0:
                cout << "Returning to main menu...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);
}

// ============================================================
// MAIN
// ============================================================
int main() {
    // sample/dummy staff data (would come from a shared login module in the full system)
    vector<Staff> staffList = {
        {"admin", "1234", "Admin"},
        {"reception1", "pass123", "Receptionist"}
    };

    vector<BillRecord> billRecords;   // shared billing data structure

    string username, password;
    bool loggedIn = false;
    int attempts = 0;

    // do-while: allow limited login attempts
    do {
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;

        if (login(staffList, username, password)) {
            loggedIn = true;
            cout << "\nLogin successful! Welcome, " << username << ".\n";
        } else {
            attempts++;
            cout << "Invalid credentials. Attempts left: " << (MAX_LOGIN_ATTEMPTS - attempts) << "\n";
        }
    } while (!loggedIn && attempts < MAX_LOGIN_ATTEMPTS);

    if (loggedIn) {
        billingReportMenu(billRecords);
    } else {
        cout << "\nToo many failed attempts. Exiting system.\n";
    }

    cout << "\nThank you for using the Clinic Management System.\n";
    return 0;
}
