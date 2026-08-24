#ifndef BILLING_REPORT_H
#define BILLING_REPORT_H

#include "shared_data.h"

// Entry point called from main.cpp's main menu.
// Takes shared vectors by reference so billing can read patient and
// consultation data, and read/update the medicine inventory stock.
void billingReportMenu(vector<BillRecord>& billRecords,
                        vector<Patient>& patients,
                        vector<Consultation>& consultations,
                        vector<Medicine>& medicineInventory);

#endif
