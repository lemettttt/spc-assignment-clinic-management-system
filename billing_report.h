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
double calculateMedicineFee(vector<MedicineItem> medicines);

// Append a complete bill to the shared billRecords vector
void addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill);

// Build a BillRecord from consultation, medicines, and patient type
void generateBill(vector<BillRecord>& billRecords, Patient patient, Consultation consult,
                   vector<MedicineItem> medicines, char patientType);

// Linear search for a bill by ID; returns index or -1
int searchBillingRecord(vector<BillRecord> billRecords, int billID);

// Process payment: validates cash amount or approves debit
bool processPayment(BillRecord& bill, double amountReceived, char paymentMethod);

// Print a formatted receipt for a bill
void printReceipt(const BillRecord& bill);

// Generate a revenue report for a specific date
void generateDailyReport(const vector<BillRecord>& billRecords, const string& date);

// Generate one of four statistical reports based on option
void generateStatisticsReport(const vector<BillRecord>& billRecords,
                               const vector<Consultation>& consultations,
                               int reportOption);

// ============================================================
// READ-ONLY HELPERS (billing does not own these records)
// ============================================================

// Check if a patient exists in the shared Patient list
int findPatientIndex(const vector<Patient>& patients, int patientID);

// Find the patient's most recent consultation (last match)
int findLatestConsultation(const vector<Consultation>& consultations, int patientID);

// Exact match lookup by consultationID (precise linking)
int findConsultationByID(const vector<Consultation>& consultations, int consultationID);

// Look up a medicine ID in the shared inventory
int findMedicineIndex(const vector<Medicine>& medicineInventory, int medicineID);

#endif