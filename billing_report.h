#ifndef BILLING_REPORT_H
#define BILLING_REPORT_H

#include "clinic_shared.h"

using namespace std;

// ============================================================
// BILLING FUNCTIONS
// ============================================================

// Entry point called from main.cpp's main menu
void billingReportMenu(vector<BillRecord>& billRecords,
                        vector<Patient>& patients,
                        vector<Consultation>& consultations,
                        vector<Medicine>& medicineInventory);

// Calculate consultation fee based on doctor type (General/Specialist)
double calculateConsultationFee(string doctorType);

// Sum cost of all prescribed medicine items
double calculateMedicineFee(const vector<MedicineItem>& medicines);

// Append a complete bill to the shared billRecords vector
void addBillingRecord(vector<BillRecord>& billRecords, const BillRecord& newBill);

// Build a BillRecord from consultation (auto-uses prescribed medicines)
void generateBill(vector<BillRecord>& billRecords, const Patient& patient,
                   const Consultation& consult, char patientType);

// Linear search for a bill by ID; returns index or -1
int searchBillingRecord(const vector<BillRecord>& billRecords, int billID);

// Process payment: validates cash amount or approves debit
bool processPayment(BillRecord& bill, double amountReceived, char paymentMethod);

// Print a formatted receipt for a bill with medicine details
void printReceipt(const BillRecord& bill);

// Generate a revenue report for a specific date
void generateDailyReport(const vector<BillRecord>& billRecords, const string& date);

// Generate one of four statistical reports based on option
void generateStatisticsReport(const vector<BillRecord>& billRecords,
                               const vector<Consultation>& consultations,
                               int reportOption);

#endif