/*
    ============================================================
    Billing & Reporting Module
    Soo Hui Xin
    ============================================================
 */

#include <iostream>
#include <iomanip>
#include "clinic_shared.h"
#include "input_utils.h"
#include "billing_report.h"

using namespace std;

// ============================================================
// SEARCH BILLING RECORD
// ============================================================

int searchBillingRecord(const vector<BillRecord>& billRecords, int billID) {
    for (size_t i = 0; i < billRecords.size(); ++i) {
        if (billRecords[i].billID == billID) return static_cast<int>(i);
    }
    return -1;
}

// ============================================================
// PRIVATE HELPER FUNCTIONS
// ============================================================

// Validate and get Bill ID from user, return index or -1 if cancelled
static int getValidatedBillIndex(const vector<BillRecord>& billRecords, bool allowCancel = true) {
    while (true) {
        string prompt = allowCancel ? "Enter Bill ID (0 to cancel): " : "Enter Bill ID: ";
        int billID = getValidatedInt(prompt);

        if (allowCancel && billID == 0) return -1;

        if (billID < 0) {
            cout << "  [ERROR] Please enter a positive number or 0 to cancel.\n";
            continue;
        }

        int idx = searchBillingRecord(billRecords, billID);
        if (idx != -1) return idx;
        cout << "  Bill not found! Please try again.\n";
    }
}

// Validate and get Patient ID, return patient index or -1
static int getValidatedPatientIndex(const vector<Patient>& patients, bool allowCancel = true) {
    while (true) {
        string prompt = allowCancel ? "Enter Patient ID (0 to cancel): " : "Enter Patient ID: ";
        int patientID = getValidatedInt(prompt);

        if (allowCancel && patientID == 0) return -1;

        if (patientID < 0) {
            cout << "  [ERROR] Please enter a positive number or 0 to cancel.\n";
            continue;
        }

        int idx = findPatientIndex(patients, patientID);
        if (idx != -1) return idx;
        cout << "  Patient not found! Please try again.\n";
    }
}

// Get the latest consultation for a patient, return index or -1
static int getValidatedConsultationIndex(const vector<Consultation>& consultations, int patientID) {
    int idx = findLatestConsultation(consultations, patientID);

    if (idx == -1) {
        cout << "  No consultation record found for this patient!\n"
             << "  Please record a consultation first.\n";
    }

    return idx;
}

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

double calculateMedicineFee(const vector<MedicineItem>& medicines) {
    double total = 0.0;
    for (const auto& med : medicines) {
        total += med.qty * med.price;
    }
    return total;
}

void addBillingRecord(vector<BillRecord>& billRecords, const BillRecord& newBill) {
    billRecords.push_back(newBill);
    cout << "\n  ==========================================\n";
    cout << "  Bill generated successfully.\n";
    cout << "  Bill ID: " << newBill.billID << endl;
    cout << "  Total Amount: RM " << fixed << setprecision(2) << newBill.totalAmount << endl;
    cout << "  ==========================================\n";
}

void generateBill(vector<BillRecord>& billRecords, const Patient& patient,
                   const Consultation& consult, char patientType) {
    BillRecord newBill;

    // Bill ID starts from 4000
    newBill.billID         = 4000 + static_cast<int>(billRecords.size()) + 1;
    newBill.patientID      = patient.patientID;
    newBill.doctorID       = consult.doctorID;
    newBill.consultationID = consult.consultationID;
    newBill.serviceDate    = consult.consultationDate;
    newBill.isPaid         = false;

    // ============================================================
    // Auto-convert prescribedMedicines from Consultation to MedicineItem
    // ============================================================
    for (const auto& prescribed : consult.prescribedMedicines) {
        MedicineItem item;
        item.name  = prescribed.medicineName;
        item.qty   = prescribed.quantity;
        item.price = 0.0;   // default, will be updated if found

        // Find price from medicine inventory
        int mIdx = findMedicineIndex(medicines, prescribed.medicineID);
        if (mIdx != -1) {
            item.price = medicines[mIdx].price;
        } else {
            cout << "  Warning: Medicine ID " << prescribed.medicineID
                 << " not found in inventory. Price set to RM 0.00\n";
        }

        newBill.medicines.push_back(item);
    }

    newBill.consultationFee = calculateConsultationFee(consult.doctorType);
    newBill.medicineFee     = calculateMedicineFee(newBill.medicines);

    double discount = 0.0;
    if (patientType == 'S' || patientType == 's') {
        discount = newBill.consultationFee * 0.10;   // Senior: 10% off
    } else if (patientType == 'M' || patientType == 'm') {
        discount = newBill.consultationFee * 0.05;   // Member: 5% off
    } else {
        discount = 0.0;                               // Normal: no discount
    }
    newBill.discount = discount;

    double subtotal = newBill.consultationFee + newBill.medicineFee - discount;
    newBill.sst         = subtotal * SST_RATE;        // SST_RATE = 0.06 (6%)
    newBill.totalAmount = subtotal + newBill.sst;
    newBill.amountPaid  = 0.0;
    newBill.changeGiven = 0.0;
    newBill.isPaid      = false;

    addBillingRecord(billRecords, newBill);
}

bool processPayment(BillRecord& bill, double amountReceived, char paymentMethod) {
    if (bill.isPaid) {
        cout << "  This bill has already been paid! Duplicate payment rejected.\n";
        return false;
    }

    if (paymentMethod == 'C' || paymentMethod == 'c') {
        if (amountReceived >= bill.totalAmount) {
            bill.amountPaid  = amountReceived;
            bill.changeGiven = amountReceived - bill.totalAmount;
            bill.isPaid      = true;
            cout << fixed << setprecision(2);
            cout << "  Payment successful. Change: RM " << bill.changeGiven << endl;
            return true;
        } else {
            cout << "  Insufficient payment! Amount short by RM "
                 << fixed << setprecision(2) << (bill.totalAmount - amountReceived) << endl;
            return false;
        }
    } else if (paymentMethod == 'D' || paymentMethod == 'd') {
        bill.amountPaid  = bill.totalAmount;
        bill.changeGiven = 0.0;
        bill.isPaid      = true;
        cout << "  Payment approved via debit/card." << endl;
        return true;
    } else {
        cout << "  Invalid payment method!" << endl;
        return false;
    }
}

void printReceipt(const BillRecord& bill) {
    cout << fixed << setprecision(2);

    cout << "\n  =========================================\n";
    cout << "              CLINIC RECEIPT             \n";
    cout << "  =========================================\n";
    cout << "  Bill ID      : " << bill.billID << endl;
    cout << "  Patient ID   : " << bill.patientID << endl;
    cout << "  Date         : " << bill.serviceDate << endl;
    cout << "  ----------------------------------------\n";

    cout << "  " << setw(22) << left << "Consultation Fee" << "RM " << setw(8) << right << bill.consultationFee << endl;

    if (!bill.medicines.empty()) {
        cout << "\n  Medicines:\n";
        cout << "  " << setw(25) << left << "Name"
             << setw(8) << right << "Qty"
             << setw(12) << right << "Price"
             << setw(12) << right << "Subtotal" << endl;
        cout << "  " << string(57, '-') << endl;

        for (const auto& med : bill.medicines) {
            double subtotal = med.qty * med.price;
            cout << "  " << setw(25) << left << med.name
                 << setw(8) << right << med.qty
                 << "   " << setw(8) << right << "RM " << med.price
                 << setw(8) << right << "RM " << subtotal << endl;
        }
        cout << "  " << string(57, '-') << endl;
    }

    cout << "  " << setw(22) << left << "Medicine Fee"      << "RM " << setw(8) << right << bill.medicineFee << endl;
    cout << "  " << setw(22) << left << "Discount"          << "RM " << setw(8) << right << bill.discount << endl;
    cout << "  " << setw(22) << left << "SST (6%)"          << "RM " << setw(8) << right << bill.sst << endl;

    cout << "  ----------------------------------------\n";

    cout << "  " << setw(22) << left << "Total Payable"     << "RM " << setw(8) << right << bill.totalAmount << endl;
    cout << "  " << setw(22) << left << "Amount Paid"       << "RM " << setw(8) << right << bill.amountPaid << endl;
    cout << "  " << setw(22) << left << "Change"            << "RM " << setw(8) << right << bill.changeGiven << endl;

    cout << "  =========================================\n";
    cout << "  " << (bill.isPaid ? "Status: PAID" : "Status: UNPAID") << "\n\n";
}

void generateDailyReport(const vector<BillRecord>& billRecords, const string& date) {
    double totalRevenue = 0.0;
    int count = 0;

    cout << fixed << setprecision(2);

    cout << "\n  =========================================\n";
    cout << "        DAILY REVENUE REPORT (" << date << ")\n";
    cout << "  =========================================\n";

    cout << "  " << setw(10) << left << "Bill ID" << setw(15) << "Patient ID" << "Total (RM)" << endl;
    cout << "  ----------------------------------------\n";

    for (const auto& bill : billRecords) {
        if (bill.serviceDate == date) {
            cout << "  " << setw(10) << left << bill.billID
                 << setw(15) << bill.patientID
                 << bill.totalAmount << endl;
            totalRevenue += bill.totalAmount;
            count++;
        }
    }

    cout << "  ----------------------------------------\n";
    cout << "  Total Patients Billed : " << count << endl;
    cout << "  Total Revenue         : RM " << totalRevenue << endl;
    cout << "  =========================================\n";
}

void generateStatisticsReport(const vector<BillRecord>& billRecords,
                               const vector<Consultation>& consultations,
                               int reportOption) {
    cout << fixed << setprecision(2);

    switch (reportOption) {
        case 1: {  // Disease statistics
            cout << "\n  ----- DISEASE STATISTICS REPORT -----\n";
            vector<string> diseases;
            vector<int> caseCount;

            for (const auto& bill : billRecords) {
                int cIdx = findConsultationByID(consultations, bill.consultationID);
                if (cIdx == -1) continue;

                string d = consultations[cIdx].diagnosis;
                if (d.empty()) continue;

                int idx = -1;
                for (size_t j = 0; j < diseases.size(); ++j) {
                    if (diseases[j] == d) { idx = static_cast<int>(j); break; }
                }
                if (idx == -1) {
                    diseases.push_back(d);
                    caseCount.push_back(1);
                } else {
                    caseCount[idx]++;
                }
            }

            if (diseases.empty()) {
                cout << "  No diagnosis data available yet.\n";
            } else {
                for (size_t i = 0; i < diseases.size(); ++i) {
                    cout << "  " << diseases[i] << " : " << caseCount[i] << " cases\n";
                }
            }
            break;
        }

        case 2: {  // Doctor workload
            cout << "\n  ----- DOCTOR WORKLOAD REPORT -----\n";
            vector<int> doctorIDs;
            vector<int> visitCount;

            for (const auto& bill : billRecords) {
                int id = bill.doctorID;
                int idx = -1;
                for (size_t j = 0; j < doctorIDs.size(); ++j) {
                    if (doctorIDs[j] == id) { idx = static_cast<int>(j); break; }
                }
                if (idx == -1) {
                    doctorIDs.push_back(id);
                    visitCount.push_back(1);
                } else {
                    visitCount[idx]++;
                }
            }

            for (size_t i = 0; i < doctorIDs.size(); ++i) {
                cout << "  Doctor ID " << doctorIDs[i] << " : " << visitCount[i] << " patients seen\n";
            }
            break;
        }

        case 3: {  // Revenue summary
            double consultTotal = 0, medTotal = 0, sstTotal = 0, grandTotal = 0;
            for (const auto& bill : billRecords) {
                consultTotal += bill.consultationFee;
                medTotal     += bill.medicineFee;
                sstTotal     += bill.sst;
                grandTotal   += bill.totalAmount;
            }
            cout << "\n  ----- REVENUE SUMMARY REPORT -----\n";
            cout << "  Consultation Fees : RM " << consultTotal << endl;
            cout << "  Medicine Sales    : RM " << medTotal << endl;
            cout << "  SST Collected     : RM " << sstTotal << endl;
            cout << "  Total Revenue     : RM " << grandTotal << endl;
            break;
        }

        case 4: {  // Medicine usage
            cout << "\n  ----- MEDICINE USAGE REPORT -----\n";
            vector<string> names;
            vector<int> quantities;

            for (const auto& bill : billRecords) {
                for (const auto& med : bill.medicines) {
                    string name = med.name;
                    int qty = med.qty;
                    int idx = -1;
                    for (size_t k = 0; k < names.size(); ++k) {
                        if (names[k] == name) { idx = static_cast<int>(k); break; }
                    }
                    if (idx == -1) {
                        names.push_back(name);
                        quantities.push_back(qty);
                    } else {
                        quantities[idx] += qty;
                    }
                }
            }

            for (size_t i = 0; i < names.size(); ++i) {
                cout << "  " << names[i] << " : " << quantities[i] << " units sold\n";
            }
            break;
        }

        default:
            cout << "  Invalid report option!\n";
    }
    cout << "  =========================================\n";
}

// ============================================================
// MAIN MENU
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
            case 1: {  // Generate Bill
                int pIdx = getValidatedPatientIndex(patients, true);
                if (pIdx == -1) {
                    cout << "  Operation cancelled.\n";
                    waitForEnter();
                    break;
                }

                Patient patient = patients[pIdx];

                int cIdx = getValidatedConsultationIndex(consultations, patient.patientID);
                if (cIdx == -1) {
                    waitForEnter();
                    break;
                }

                Consultation consult = consultations[cIdx];

                // ============================================================
                // Show medicines from consultation
                // ============================================================
                if (consult.prescribedMedicines.empty()) {
                    cout << "\n  No medicines prescribed in this consultation.\n";
                    cout << "  Bill will be generated with medicine fee: RM 0.00\n\n";
                } else {
                    cout << "\n  Medicines from consultation:\n";
                    for (const auto& med : consult.prescribedMedicines) {
                        cout << "    - " << med.medicineName << " x" << med.quantity << endl;
                    }
                    cout << endl;
                }

                string patientTypeInput;
                while (true) {
                    patientTypeInput = getNonEmptyString("  Patient Type (S=Senior, M=Member, N=Normal): ");
                    if (patientTypeInput.length() == 1) {
                        char c = toupper(patientTypeInput[0]);
                        if (c == 'S' || c == 'M' || c == 'N') break;
                    }
                    cout << "  [ERROR] Invalid input. Please enter S, M, or N.\n";
                }
                char patientType = toupper(patientTypeInput[0]);

                generateBill(billRecords, patient, consult, patientType);
                waitForEnter();
                break;
            }

            case 2: {  // Process Payment
                int idx = getValidatedBillIndex(billRecords, true);
                if (idx == -1) {
                    pressZeroToExit();
                    break;
                }

                cout << fixed << setprecision(2);
                cout << "  Bill Total: RM " << billRecords[idx].totalAmount << endl;

                double amount = getNonNegativeDouble("  Enter payment amount (RM) (0 to cancel): ");
                if (amount == 0) {
                    cout << "  Payment cancelled.\n";
                    waitForEnter();
                    break;
                }

                string methodInput;
                while (true) {
                    methodInput = getNonEmptyString("  Payment method (C=Cash, D=Debit, 0=cancel): ");
                    if (methodInput.length() == 1) {
                        char c = toupper(methodInput[0]);
                        if (c == '0') {
                            cout << "  Payment cancelled.\n";
                            waitForEnter();
                            break;
                        }
                        if (c == 'C' || c == 'D') break;
                    }
                    cout << "  [ERROR] Invalid input. Please enter C, D, or 0.\n";
                }
                if (methodInput == "0") break;

                char method = toupper(methodInput[0]);

                processPayment(billRecords[idx], amount, method);
                waitForEnter();
                break;
            }

            case 3: {  // Search Bill
                int idx = getValidatedBillIndex(billRecords, true);
                if (idx == -1) {
                    cout << "  Search cancelled.\n";
                    waitForEnter();
                    break;
                }
                cout << "  Bill found at record.\n";
                waitForEnter();
                break;
            }

            case 4: {  // Print Receipt
                int idx = getValidatedBillIndex(billRecords, true);
                if (idx == -1) {
                    cout << "  Print cancelled.\n";
                    waitForEnter();
                    break;
                }
                printReceipt(billRecords[idx]);
                waitForEnter();
                break;
            }

            case 5: {  // Generate Daily Report
                string dateInput;
                while (true) {
                    cout << "  Enter date (DD/MM/YYYY, 0 to cancel): ";
                    getline(cin, dateInput);
                    if (dateInput.empty()) {
                        cout << "  [ERROR] Input cannot be empty.\n";
                        continue;
                    }
                    if (dateInput == "0") {
                        cout << "  Report generation cancelled.\n";
                        waitForEnter();
                        break;
                    }
                    if (!isValidDate(dateInput)) {
                        cout << "  [ERROR] Invalid date format. Please use DD/MM/YYYY.\n";
                        continue;
                    }
                    generateDailyReport(billRecords, dateInput);
                    pressZeroToExit();
                    break;
                }
                break;
            }

            case 6: {  // Generate Statistics Report
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

                setColor(COLOR_RED);
                cout << "  |   0.  Back                                               |\n";

                setColor(COLOR_DARK_BLUE);
                cout << "  |                                                          |\n";
                cout << "  ============================================================\n";
                setColor(COLOR_RESET);

                int option = getValidatedMenuChoice("  Enter your choice (0 to back): ", 0, 4);

                if (option == 0) {
                    break;
                }

                generateStatisticsReport(billRecords, consultations, option);
                pressZeroToExit();
                break;
            }

            case 0:
                clearScreen(); 
                break;

            default:
                cout << "  Invalid choice! Please try again.\n";
                waitForEnter();
        }

    } while (choice != 0);
}