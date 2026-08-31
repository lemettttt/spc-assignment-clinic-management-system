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
// PRIVATE HELPER FUNCTIONS (reduce redundancy)
// ============================================================

// Validate and get Bill ID from user, return index or -1 if cancelled
static int getValidatedBillIndex(const vector<BillRecord>& billRecords, bool allowCancel = true) {
    while (true) {
        string prompt = allowCancel ? "Enter Bill ID (0 to cancel): " : "Enter Bill ID: ";
        int billID = getPositiveInt(prompt);

        if (allowCancel && billID == 0) return -1;

        int idx = searchBillingRecord(billRecords, billID);

        if (idx != -1) return idx;

        cout << "Bill not found! Please try again.\n";
    }
}

// Validate and get Patient ID, return patient index or -1
static int getValidatedPatientIndex(const vector<Patient>& patients, bool allowCancel = false) {
    while (true) {
        string prompt = allowCancel ? "Enter Patient ID (0 to cancel): " : "Enter Patient ID: ";
        int patientID = getPositiveInt(prompt);

        if (allowCancel && patientID == 0) return -1;

        int idx = findPatientIndex(patients, patientID);

        if (idx != -1) return idx;

        cout << "Patient not found! Please try again.\n";
    }
}

// Get the latest consultation for a patient, return index or -1
static int getValidatedConsultationIndex(const vector<Consultation>& consultations, int patientID) {
    int idx = findLatestConsultation(consultations, patientID);

    if (idx == -1) {
        cout << "No consultation record found for this patient! "
             << "Please record a consultation first.\n";
    }

    return idx;
}

// Collect prescribed medicines from user input
static vector<MedicineItem> collectPrescribedMedicines(const vector<Medicine>& medicineInventory) {
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
        item.name = medicineInventory[mIdx].medicineName;
        item.price = medicineInventory[mIdx].price;
        item.qty = qty;

        medicines.push_back(item);
    }

    return medicines;
}

// ============================================================
// BILLING FUNCTIONS
// ============================================================

// Returns consultation fee based on doctor type using named constants.
double calculateConsultationFee(string doctorType) {
    if (doctorType == "Specialist") {
        return SPECIALIST_CONSULT_FEE;
    } else {
        return GENERAL_CONSULT_FEE;
    }
}

// Sums cost of all prescribed medicine items.
double calculateMedicineFee(vector<MedicineItem> medicines) {
    double total = 0;

    for (int i = 0; i < (int)medicines.size(); i++) {
        total += medicines[i].qty * medicines[i].price;
    }

    return total;
}

// Appends a complete bill to the shared billRecords vector.
void addBillingRecord(vector<BillRecord>& billRecords, BillRecord newBill) {
    billRecords.push_back(newBill);
    cout << "\nBill generated successfully. Bill ID: " << newBill.billID << endl;
}

// Builds a BillRecord from consultation, medicines, and patient type.
void generateBill(vector<BillRecord>& billRecords, Patient patient, Consultation consult,
                   vector<MedicineItem> medicines, char patientType) {
    BillRecord newBill;

    newBill.billID         = 1000 + (int)billRecords.size() + 1;
    newBill.patientID      = patient.patientID;
    newBill.doctorID       = consult.doctorID;
    newBill.consultationID = consult.consultationID;    // exact link
    newBill.serviceDate    = consult.consultationDate;
    newBill.medicines      = medicines;

    newBill.consultationFee = calculateConsultationFee(consult.doctorType);
    newBill.medicineFee     = calculateMedicineFee(medicines);

    // Determine discount based on patient category
    double discount = 0;

    if (patientType == 'S' || patientType == 's') {
        discount = newBill.consultationFee * 0.10;   // Senior: 10% off
    } else if (patientType == 'M' || patientType == 'm') {
        discount = newBill.consultationFee * 0.05;   // Member: 5% off
    } else {
        discount = 0;                                 // Normal: no discount
    }

    newBill.discount = discount;

    // Calculate subtotal, SST, and final total
    double subtotal = newBill.consultationFee + newBill.medicineFee - discount;

    newBill.sst         = subtotal * SST_RATE;
    newBill.totalAmount = subtotal + newBill.sst;
    newBill.amountPaid  = 0;
    newBill.changeGiven = 0;
    newBill.isPaid      = false;

    addBillingRecord(billRecords, newBill);
}

// Linear search for a bill by ID; returns index or -1.
int searchBillingRecord(vector<BillRecord> billRecords, int billID) {
    for (int i = 0; i < (int)billRecords.size(); i++) {
        if (billRecords[i].billID == billID) return i;
    }

    return -1;
}

// Processes payment: validates amount for cash, or approves debit.
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

// Prints a formatted receipt for a bill.
void printReceipt(const BillRecord& bill) {
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

// Generates a revenue report for a specific date.
void generateDailyReport(const vector<BillRecord>& billRecords, const string& date) {
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

// Generates one of four statistical reports based on option.
void generateStatisticsReport(const vector<BillRecord>& billRecords,
                               const vector<Consultation>& consultations,
                               int reportOption) {
    cout << fixed << setprecision(2);

    switch (reportOption) {
        case 1: {  // Disease statistics
            cout << "\n----- DISEASE STATISTICS REPORT -----\n";

            vector<string> diseases;
            vector<int> caseCount;

            for (int i = 0; i < (int)billRecords.size(); i++) {
                int cIdx = findConsultationByID(consultations, billRecords[i].consultationID);

                if (cIdx == -1) continue;

                string d = consultations[cIdx].diagnosis;

                if (d.empty()) continue;

                int idx = -1;

                for (int j = 0; j < (int)diseases.size(); j++) {
                    if (diseases[j] == d) { idx = j; break; }
                }

                if (idx == -1) {
                    diseases.push_back(d);
                    caseCount.push_back(1);
                } else {
                    caseCount[idx]++;
                }
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
                    pressZeroToContinue();
                    break;
                }

                Patient patient = patients[pIdx];

                int cIdx = getValidatedConsultationIndex(consultations, patient.patientID);

                if (cIdx == -1) {
                    pressZeroToContinue();
                    break;
                }

                Consultation consult = consultations[cIdx];

                vector<MedicineItem> medicines = collectPrescribedMedicines(medicineInventory);

                string patientTypeInput;

                while (true) {
                    patientTypeInput = getNonEmptyString("Patient Type (S=Senior, M=Member, N=Normal): ");

                    if (patientTypeInput.length() == 1) {
                        char c = toupper(patientTypeInput[0]);

                        if (c == 'S' || c == 'M' || c == 'N') break;
                    }

                    cout << "[ERROR] Invalid input. Please enter S, M, or N.\n";
                }

                char patientType = toupper(patientTypeInput[0]);

                generateBill(billRecords, patient, consult, medicines, patientType);
                waitForEnter();

                break;
            }

            case 2: {  // Process Payment
                int idx = getValidatedBillIndex(billRecords, true);

                if (idx == -1) {
                    pressZeroToContinue();
                    break;
                }

                double amount = getNonNegativeDouble("Enter payment amount (RM): ");

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
                waitForEnter();

                break;
            }

            case 3: {  // Search Bill
                int idx = getValidatedBillIndex(billRecords, true);

                if (idx == -1) {
                    pressZeroToContinue();
                    break;
                }

                cout << "Bill found at record.\n";
                pressZeroToContinue();

                break;
            }

            case 4: {  // Print Receipt
                int idx = getValidatedBillIndex(billRecords, true);

                if (idx == -1) {
                    pressZeroToContinue();
                    break;
                }

                printReceipt(billRecords[idx]);
                pressZeroToContinue();

                break;
            }

            case 5: {  // Generate Daily Report
                string date = getValidatedDate("Enter date (DD/MM/YYYY): ");
                generateDailyReport(billRecords, date);
                pressZeroToContinue();

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
                pressZeroToContinue();

                break;
            }

            case 0:
                cout << "Returning to main menu...\n";
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
                pressZeroToContinue();
        }

    } while (choice != 0);
}