/*
    ============================================================
    billing_report.cpp
    Module: Billing & Reporting
    Author: Lemet
    ============================================================
    This module reads shared data owned by teammates' modules
    (Patient, Consultation, Medicine) but only WRITES to its own
    shared BillRecord vector. It never modifies patient records,
    consultation records, or medicine stock directly.
    ============================================================
*/

#include <iostream>
#include <iomanip>
#include "clinic_shared.h" 
#include "input_utils.h"

using namespace std;

// ---------------- FUNCTION PROTOTYPES ----------------
double calculateConsultationFee(string doctorType);
double calculateMedicineFee(vector<MedicineItem> medicines);
void   addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill);
void   generateBill(vector<BillRecord>& billRecords, Patient patient, Consultation consult,
                     vector<MedicineItem> medicines, char patientType);
int    searchBillingRecord(vector<BillRecord> billRecords, int billID);
bool   processPayment(BillRecord& bill, double amountReceived, char paymentMethod);
void   printReceipt(BillRecord bill);
void   generateDailyReport(vector<BillRecord> billRecords, string date);
void   generateStatisticsReport(vector<BillRecord> billRecords, vector<Consultation> consultations, int reportOption);

// read-only helpers over shared data (billing does not own these records)
int findPatientIndex(vector<Patient> patients, int patientID);
int findLatestConsultation(vector<Consultation> consultations, int patientID);
int findConsultationByID(vector<Consultation> consultations, int consultationID);
int findMedicineIndex(vector<Medicine> medicineInventory, int medicineID);

// ============================================================
// BILLING FUNCTIONS
// ============================================================

// STEP 1：calculateConsultationFee()
// Returns the consultation fee for the given doctor type.
// Uses named constants (GENERAL_CONSULT_FEE / SPECIALIST_CONSULT_FEE)
// instead of hardcoding the fee values.
double calculateConsultationFee(string doctorType) {
    if (doctorType == "Specialist") {
        return SPECIALIST_CONSULT_FEE;
    } else {
        return GENERAL_CONSULT_FEE;
    }
}

// STEP 2：calculateMedicineFee()
// Sums up the cost of every prescribed medicine item on a bill.
// Takes the vector by value since it only reads quantity/price,
// it never needs to modify the caller's medicine list.
double calculateMedicineFee(vector<MedicineItem> medicines) {
    double total = 0;
    for (int i = 0; i < (int)medicines.size(); i++) {
        total += medicines[i].qty * medicines[i].price;
    }
    return total;
}

// STEP 3：addBillingRecord()
// Appends a fully-built bill to the shared billRecords vector.
// billRecords is passed by reference because the shared list must
// actually be updated for the rest of the system to see the new bill.
void addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill) {
    billRecords.push_back(newBill);
    cout << "\nBill generated successfully. Bill ID: " << newBill.billID << endl;
}

// STEP 4：generateBill()
// Builds a complete BillRecord for a patient's visit by combining
// consultation fee, medicine fee, patient-type discount, and SST.
// patient/consult are passed by value (read-only snapshots used to
// populate the bill); billRecords is passed by reference since the
// finished bill must be saved into the shared vector.
void generateBill(vector<BillRecord>& billRecords, Patient patient, Consultation consult,
                   vector<MedicineItem> medicines, char patientType) {
    BillRecord newBill;
    newBill.billID        = 1000 + (int)billRecords.size() + 1;
    newBill.patientID     = patient.patientID;
    newBill.doctorID      = consult.doctorID;
    newBill.consultationID = consult.consultationID;    // exact link, not a "latest guess"
    newBill.serviceDate   = consult.consultationDate;   // 适配 Consultation 结构体
    // 注意：BillRecord 没有 diagnosis 字段，所以这行已删除
    newBill.medicines     = medicines;

    // Return values from calculateConsultationFee() and calculateMedicineFee()
    // are combined here to build up the final total below.
    newBill.consultationFee = calculateConsultationFee(consult.doctorType);
    newBill.medicineFee     = calculateMedicineFee(medicines);

    // Selection: if-else-if determines the discount rate by patient category.
    double discount = 0;
    if (patientType == 'S' || patientType == 's') {
        discount = newBill.consultationFee * 0.10;   // Senior: 10% off consultation fee
    } else if (patientType == 'M' || patientType == 'm') {
        discount = newBill.consultationFee * 0.05;   // Member: 5% off consultation fee
    } else {
        discount = 0;                                 // Normal: no discount
    }
    newBill.discount = discount;

    // Calculation chain: subtotal -> SST -> final total payable.
    double subtotal = newBill.consultationFee + newBill.medicineFee - discount;
    newBill.sst          = subtotal * SST_RATE;
    newBill.totalAmount  = subtotal + newBill.sst;
    newBill.amountPaid   = 0;
    newBill.changeGiven  = 0;
    newBill.isPaid       = false;

    addBillingRecord(billRecords, newBill);
}

// STEP 5：searchBillingRecord()
// Linear search for a bill by its Bill ID.
// Returns the matching index, or -1 if no bill has that ID.
int searchBillingRecord(vector<BillRecord> billRecords, int billID) {
    for (int i = 0; i < (int)billRecords.size(); i++) {
        if (billRecords[i].billID == billID) return i;
    }
    return -1;
}

// STEP 6：findPatientIndex()
// Read-only lookup into the shared Patient Management data.
// Confirms a patientID actually exists before billing uses it.
int findPatientIndex(vector<Patient> patients, int patientID) {
    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].patientID == patientID) return i;
    }
    return -1;
}

// STEP 7：findLatestConsultation()
// Read-only lookup into the shared Consultation data.
// Keeps overwriting 'latest' on every match, so after the loop it
// naturally holds the index of the patient's most recent consultation.
int findLatestConsultation(vector<Consultation> consultations, int patientID) {
    int latest = -1;
    for (int i = 0; i < (int)consultations.size(); i++) {
        if (consultations[i].patientID == patientID) latest = i;  // last match = most recent
    }
    return latest;
}

// STEP 7b：findConsultationByID()
// Precise, exact-match lookup by consultationID — used once a BillRecord
// already knows exactly which consultation it was billed for, instead of
// re-guessing "the patient's latest consultation" after the fact.
int findConsultationByID(vector<Consultation> consultations, int consultationID) {
    for (int i = 0; i < (int)consultations.size(); i++) {
        if (consultations[i].consultationID == consultationID) return i;
    }
    return -1;
}

// STEP 8：findMedicineIndex()
// Read-only lookup into the shared Medicine Inventory data.
// Used to pull the current name/price for a medicine ID during billing.
int findMedicineIndex(vector<Medicine> medicineInventory, int medicineID) {
    for (int i = 0; i < (int)medicineInventory.size(); i++) {
        if (medicineInventory[i].medicineID == medicineID) return i;
    }
    return -1;
}

// STEP 9：processPayment()
// Validates and applies a payment to a specific bill.
// bill is passed by reference because isPaid/amountPaid/changeGiven
// must be updated on the actual record, not a copy.
bool processPayment(BillRecord& bill, double amountReceived, char paymentMethod) {
    // Guard clause: reject payment if this bill was already settled.
    if (bill.isPaid) {
        cout << "This bill has already been paid! Duplicate payment rejected.\n";
        return false;
    }

    // Nested if: outer checks payment method, inner (for cash) checks
    // whether the amount received is enough before computing change.
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

// STEP 10：printReceipt()
// Displays a formatted receipt for one bill. Takes the bill by value
// since this function only displays information, it never edits it.
// setw()/left/right/fixed/setprecision(2) are used together to align
// the receipt into readable, professionally formatted columns.
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

// STEP 11：generateDailyReport()
// Filter-then-accumulate pattern: loops through every bill, and for
// bills matching the given date, adds to the running patient count
// and revenue total.
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

// STEP 12：generateStatisticsReport()
// Switch statement routes to one of four management-level reports
// from a single function, instead of writing four near-identical
// functions. Each case uses its own loop (and, for medicine usage,
// a nested loop) to group and total the relevant data.
void generateStatisticsReport(vector<BillRecord> billRecords, vector<Consultation> consultations, int reportOption) {
    cout << fixed << setprecision(2);
    switch (reportOption) {
        case 1: {  // Disease statistics
            cout << "\n----- DISEASE STATISTICS REPORT -----\n";
            vector<string> diseases;
            vector<int> caseCount;
            // For each bill, look up its EXACT consultation record via the
            // consultationID stored on the bill at generation time — not a
            // "latest consultation" guess, so history stays correct even
            // if the patient has since had a newer consultation.
            for (int i = 0; i < (int)billRecords.size(); i++) {
                int cIdx = findConsultationByID(consultations, billRecords[i].consultationID);
                if (cIdx == -1) continue;              // consultation record no longer exists
                string d = consultations[cIdx].diagnosis;
                if (d.empty()) continue;                // diagnosis not recorded

                // Group by diagnosis name (linear search + accumulate).
                int idx = -1;
                for (int j = 0; j < (int)diseases.size(); j++) {
                    if (diseases[j] == d) { idx = j; break; }
                }
                if (idx == -1) { diseases.push_back(d); caseCount.push_back(1); }
                else { caseCount[idx]++; }
            }
            if (diseases.empty()) {
                cout << "No diagnosis data available yet.\n";
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
            // Group bills by doctorID: linear-search doctorIDs seen so
            // far, and either start a new count or increment it.
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
            // Nested loop: for every medicine item on every bill, group
            // by medicine name and accumulate the total quantity sold.
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
// ------------------------------------------------------------
// clearScreen() at the top of the loop wipes the screen for a fresh
// menu on every pass. pressZeroToContinue() is used after each case
// prints its result, so the output stays visible for the user to
// read BEFORE the next loop iteration clears it away.
// ============================================================
void billingReportMenu(vector<BillRecord>& billRecords, vector<Patient>& patients,
                        vector<Consultation>& consultations, vector<Medicine>& medicineInventory) {
    int choice;
    do {
        clearScreen();

        setColor(COLOR_DARK_BLUE);
        cout << "\n  ============================================================\n";
        cout << "  |                BILLING & REPORTING MODULE                 |\n";
        cout << "  +----------------------------------------------------------+\n";
        cout << "  |                                                          |\n";

        setColor(COLOR_LIGHT_BLUE);
        cout << "  |   1.  Generate Bill                                      |\n";
        cout << "  |   2.  Process Payment                                    |\n";
        cout << "  |   3.  Search Bill                                        |\n";
        cout << "  |   4.  Print Receipt                                      |\n";
        cout << "  |   5.  Generate Daily Report                              |\n";
        cout << "  |   6.  Generate Statistics Report                         |\n";

        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";

        setColor(COLOR_RED);
        cout << "  |   0.  Back to Main Menu                                  |\n";

        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        cout << "  ============================================================\n";
        setColor(COLOR_RESET);

        choice = getValidatedMenuChoice("  Enter your choice: ", 0, 6);

        switch (choice) {
            // ---------------- 1. Generate Bill ----------------
            case 1: {
                int patientID = getPositiveInt("Enter Patient ID: ");

                // Validate the patient exists before continuing (data
                // sharing with the Patient Management module).
                int pIdx = findPatientIndex(patients, patientID);
                if (pIdx == -1) {
                    cout << "Patient not found! Please register this patient in Patient Management first.\n";
                    pressZeroToContinue();
                    break;
                }
                Patient patient = patients[pIdx];

                // Validate a consultation exists for this patient (data
                // sharing with the Consultation & Medicine module) —
                // doctorID, doctorType, and service date all come from here.
                int cIdx = findLatestConsultation(consultations, patientID);
                if (cIdx == -1) {
                    cout << "No consultation record found for this patient! Please record a consultation first.\n";
                    pressZeroToContinue();
                    break;
                }
                Consultation consult = consultations[cIdx];

                // Collect prescribed medicine items; name/price are looked
                // up from the shared inventory instead of typed manually.
                vector<MedicineItem> medicines;
                int medCount = getNonNegativeInt("Number of medicine items prescribed: ");
                for (int i = 0; i < medCount; i++) {
                    int medID = getPositiveInt("  Medicine ID: ");
                    int mIdx = findMedicineIndex(medicineInventory, medID);
                    if (mIdx == -1) {
                        cout << "  Medicine not found in inventory! Skipping this item.\n";
                        continue;
                    }

                    int qty = getPositiveInt("  Quantity: ");
                    MedicineItem item;
                    item.name  = medicineInventory[mIdx].medicineName;
                    item.price = medicineInventory[mIdx].price;
                    item.qty   = qty;
                    medicines.push_back(item);
                }

                // Validation loop: only accept S / M / N (case-insensitive)
                // for patient category, re-prompting on anything else.
                string patientTypeInput;
                while (true) {
                    patientTypeInput = getNonEmptyString("Patient Type (S=Senior, M=Member, N=Normal): ");
                    if (patientTypeInput.length() == 1) {
                        char c = toupper(patientTypeInput[0]);
                        if (c == 'S' || c == 'M' || c == 'N') {
                            break;
                        }
                    }
                    cout << "[ERROR] Invalid input. Please enter S, M, or N.\n";
                }
                char patientType = toupper(patientTypeInput[0]);

                generateBill(billRecords, patient, consult, medicines, patientType);
                pressZeroToContinue();
                break;
            }

            // ---------------- 2. Process Payment ----------------
            case 2: {
                int billID = getPositiveInt("Enter Bill ID: ");
                int idx = searchBillingRecord(billRecords, billID);
                if (idx == -1) {
                    cout << "Bill not found!\n";
                    pressZeroToContinue();
                } else {
                    double amount = getNonNegativeDouble("Enter payment amount (RM): ");

                    // Validation loop: only accept C / D (case-insensitive)
                    // for payment method, re-prompting on anything else.
                    string methodInput;
                    while (true) {
                        methodInput = getNonEmptyString("Payment method (C=Cash, D=Debit): ");
                        if (methodInput.length() == 1) {
                            char c = toupper(methodInput[0]);
                            if (c == 'C' || c == 'D') break;
                        }
                        cout << "[ERROR] Invalid input. Please enter C or D.\n";
                    }
                    char method = toupper(methodInput[0]);

                    processPayment(billRecords[idx], amount, method);
                    pressZeroToContinue();
                }
                break;
            }

            // ---------------- 3. Search Bill ----------------
            case 3: {
                int billID = getPositiveInt("Enter Bill ID: ");
                int idx = searchBillingRecord(billRecords, billID);
                cout << (idx == -1 ? "Bill not found!\n" : "Bill found at record.\n");
                pressZeroToContinue();
                break;
            }

            // ---------------- 4. Print Receipt ----------------
            case 4: {
                int billID = getPositiveInt("Enter Bill ID: ");
                int idx = searchBillingRecord(billRecords, billID);
                if (idx == -1) {
                    cout << "Bill not found!\n";
                } else {
                    printReceipt(billRecords[idx]);
                }
                pressZeroToContinue();
                break;
            }

            // ---------------- 5. Generate Daily Report ----------------
            case 5: {
                string date = getValidatedDate("Enter date (DD/MM/YYYY): ");
                generateDailyReport(billRecords, date);
                pressZeroToContinue();
                break;
            }

            // ---------------- 6. Generate Statistics Report ----------------
            case 6: {
                // Transitioning to a new sub-page, so clear the screen and
                // show a properly formatted, colored selection menu instead
                // of a plain inline prompt.
                clearScreen();

                setColor(COLOR_DARK_BLUE);
                cout << "\n  ============================================================\n";
                cout << "  |                 STATISTICS REPORT MENU                    |\n";
                cout << "  +----------------------------------------------------------+\n";
                cout << "  |                                                          |\n";

                setColor(COLOR_LIGHT_BLUE);
                cout << "  |   1.  Disease Statistics                                 |\n";
                cout << "  |   2.  Doctor Workload                                    |\n";
                cout << "  |   3.  Revenue Summary                                    |\n";
                cout << "  |   4.  Medicine Usage                                     |\n";

                setColor(COLOR_DARK_BLUE);
                cout << "  |                                                          |\n";
                cout << "  ============================================================\n";
                setColor(COLOR_RESET);

                int option = getValidatedMenuChoice("  Enter your choice: ", 1, 4);
                generateStatisticsReport(billRecords, consultations, option);
                pressZeroToContinue();
                break;
            }

            // ---------------- 0. Back to Main Menu ----------------
            case 0:
                cout << "Returning to main menu...\n";
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
                pressZeroToContinue();
        }
    } while (choice != 0);
}