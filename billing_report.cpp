/*
    ============================================================
    billing_report.cpp
    Module: Billing & Reporting
    Author: Lemet
    ============================================================
    This file demonstrates the assigned module ONLY (standalone).
    When integrated with teammates' modules:
      - staffList would come from a shared login/staff module
      - patientID / doctorID would be validated against
        Patient Management and Appointment modules
      - medicines/prices could come from the Consultation &
        Medicine Inventory module

    Integration notes:
      - patients        -> owned/maintained by Patient Management (Chia Koh Suan)
      - consultations    -> owned/maintained by Consultation & Medicine
                             Inventory (Siow Pei En)
      - medicineInventory -> owned/maintained by Consultation & Medicine
                             Inventory (Siow Pei En)
      Billing only READS these vectors to build a bill; it never
      adds/edits patient, consultation, or stock records itself.
    ============================================================
*/

#include <iostream>
#include <iomanip>
#include "shared_data.h"
using namespace std;

// ---------------- CONSTANTS ----------------
const double GENERAL_CONSULT_FEE    = 45.00;
const double SPECIALIST_CONSULT_FEE = 80.00;
const double SST_RATE               = 0.06;

// ---------------- FUNCTION PROTOTYPES ----------------
double calculateConsultationFee(string doctorType);
double calculateMedicineFee(vector<MedicineItem> medicines);
void   addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill);
void   generateBill(vector<BillRecord>& billRecords, Patient patient, Consultation consult,
                     string doctorType, vector<MedicineItem> medicines, char patientType);
int    searchBillingRecord(vector<BillRecord> billRecords, int billID);
double getValidatedPayment();
bool   processPayment(BillRecord& bill, double amountReceived, char paymentMethod);
void   printReceipt(BillRecord bill);
void   generateDailyReport(vector<BillRecord> billRecords, string date);
void   generateStatisticsReport(vector<BillRecord> billRecords, int reportOption);

// read-only helpers over shared data (billing does not own these records)
int findPatientIndex(vector<Patient> patients, int patientID);
int findLatestConsultation(vector<Consultation> consultations, int patientID);
int findMedicineIndex(vector<Medicine> medicineInventory, int medicineID);

// ============================================================
// BILLING FUNCTIONS
// ============================================================
double calculateConsultationFee(string doctorType) {
    if (doctorType == "Specialist") {
        return SPECIALIST_CONSULT_FEE;
    } else {
        return GENERAL_CONSULT_FEE;
    }
}

double calculateMedicineFee(vector<MedicineItem> medicines) {
    double total = 0;
    for (int i = 0; i < (int)medicines.size(); i++) {
        total += medicines[i].qty * medicines[i].price;
    }
    return total;
}

void addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill) {
    billRecords.push_back(newBill);
    cout << "\nBill generated successfully. Bill ID: " << newBill.billID << endl;
}

void generateBill(vector<BillRecord>& billRecords, Patient patient, Consultation consult,
                   string doctorType, vector<MedicineItem> medicines, char patientType) {
    BillRecord newBill;
    newBill.billID        = 1000 + (int)billRecords.size() + 1;
    newBill.patientID     = patient.patientID;
    newBill.doctorID      = consult.doctorID;
    newBill.doctorType    = doctorType;
    newBill.serviceDate   = consult.date;       // shared data: from Consultation module
    newBill.diagnosis     = consult.diagnosis;  // shared data: from Consultation module
    newBill.medicines     = medicines;

    newBill.consultationFee = calculateConsultationFee(doctorType);
    newBill.medicineFee     = calculateMedicineFee(medicines);

    double discount = 0;
    if (patientType == 'S' || patientType == 's') {
        discount = newBill.consultationFee * 0.10;
    } else if (patientType == 'M' || patientType == 'm') {
        discount = newBill.consultationFee * 0.05;
    } else {
        discount = 0;
    }
    newBill.discount = discount;

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
        if (billRecords[i].billID == billID) return i;
    }
    return -1;
}

int findPatientIndex(vector<Patient> patients, int patientID) {
    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].patientID == patientID) return i;
    }
    return -1;
}

int findLatestConsultation(vector<Consultation> consultations, int patientID) {
    int latest = -1;
    for (int i = 0; i < (int)consultations.size(); i++) {
        if (consultations[i].patientID == patientID) latest = i;  // last match = most recent
    }
    return latest;
}

int findMedicineIndex(vector<Medicine> medicineInventory, int medicineID) {
    for (int i = 0; i < (int)medicineInventory.size(); i++) {
        if (medicineInventory[i].medicineID == medicineID) return i;
    }
    return -1;
}

double getValidatedPayment() {
    double amount;
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
    if (bill.isPaid) {
        cout << "This bill has already been paid! Duplicate payment rejected.\n";
        return false;
    }
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
    switch (reportOption) {
        case 1: {  // Disease statistics
            cout << "\n----- DISEASE STATISTICS REPORT -----\n";
            vector<string> diseases;
            vector<int> caseCount;
            for (int i = 0; i < (int)billRecords.size(); i++) {
                string d = billRecords[i].diagnosis;
                if (d == "") continue;
                int idx = -1;
                for (int j = 0; j < (int)diseases.size(); j++) {
                    if (diseases[j] == d) { idx = j; break; }
                }
                if (idx == -1) { diseases.push_back(d); caseCount.push_back(1); }
                else { caseCount[idx]++; }
            }
            for (int i = 0; i < (int)diseases.size(); i++) {
                cout << diseases[i] << " : " << caseCount[i] << " cases\n";
            }
            break;
        }
        case 2: {  // Doctor workload
            cout << "\n----- DOCTOR WORKLOAD REPORT -----\n";
            vector<int> doctorIDs;
            vector<int> visitCount;
            for (int i = 0; i < (int)billRecords.size(); i++) {
                int id = billRecords[i].doctorID;
                int idx = -1;
                for (int j = 0; j < (int)doctorIDs.size(); j++) {
                    if (doctorIDs[j] == id) { idx = j; break; }
                }
                if (idx == -1) { doctorIDs.push_back(id); visitCount.push_back(1); }
                else { visitCount[idx]++; }
            }
            for (int i = 0; i < (int)doctorIDs.size(); i++) {
                cout << "Doctor ID " << doctorIDs[i] << " : " << visitCount[i] << " patients seen\n";
            }
            break;
        }
        case 3: {  // Revenue summary
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
        case 4: {  // Medicine usage
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
                    if (idx == -1) { names.push_back(name); quantities.push_back(qty); }
                    else { quantities[idx] += qty; }
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
// MENU (do-while + switch) — called from main.cpp
// ============================================================
void billingReportMenu(vector<BillRecord>& billRecords, vector<Patient>& patients,
                        vector<Consultation>& consultations, vector<Medicine>& medicineInventory) {
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
                int patientID;
                cout << "Enter Patient ID: "; cin >> patientID;

                // integration: look up the patient in the shared vector instead of
                // trusting free-typed details (Patient Management owns this data)
                int pIdx = findPatientIndex(patients, patientID);
                if (pIdx == -1) {
                    cout << "Patient not found! Please register this patient in Patient Management first.\n";
                    break;
                }
                Patient patient = patients[pIdx];

                // integration: pull doctorID/date/diagnosis from the patient's latest
                // consultation record instead of typing them in manually
                int cIdx = findLatestConsultation(consultations, patientID);
                if (cIdx == -1) {
                    cout << "No consultation record found for this patient! Please record a consultation first.\n";
                    break;
                }
                Consultation consult = consultations[cIdx];

                string doctorType;
                cout << "Doctor Type (General/Specialist): "; cin >> doctorType;

                // integration: look up each medicine's name/price from the shared
                // inventory instead of letting the user type a price freely
                vector<MedicineItem> medicines;
                int medCount;
                cout << "Number of medicine items prescribed: "; cin >> medCount;
                for (int i = 0; i < medCount; i++) {
                    int medID, qty;
                    cout << "  Medicine ID: "; cin >> medID;
                    int mIdx = findMedicineIndex(medicineInventory, medID);
                    if (mIdx == -1) {
                        cout << "  Medicine not found in inventory! Skipping this item.\n";
                        continue;
                    }
                    cout << "  Quantity: "; cin >> qty;
                    MedicineItem item;
                    item.medicineID = medID;
                    item.name  = medicineInventory[mIdx].name;
                    item.price = medicineInventory[mIdx].price;
                    item.qty   = qty;
                    medicines.push_back(item);
                }

                char patientType;
                cout << "Patient Type (S=Senior, M=Member, N=Normal): "; cin >> patientType;

                generateBill(billRecords, patient, consult, doctorType, medicines, patientType);
                break;
            }
            case 2: {
                int billID;
                cout << "Enter Bill ID: "; cin >> billID;
                int idx = searchBillingRecord(billRecords, billID);
                if (idx == -1) {
                    cout << "Bill not found!\n";
                } else {
                    double amount = getValidatedPayment();
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
                cout << "1. Disease Statistics  2. Doctor Workload  3. Revenue Summary  4. Medicine Usage\n";
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