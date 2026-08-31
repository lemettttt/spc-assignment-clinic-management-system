#ifndef LOGIN_MODULE_H
#define LOGIN_MODULE_H

#include "clinic_shared.h"

// ============================================================
// Authentication Function Prototypes
// ============================================================

bool validatePasswordLength(const string& password);
bool checkDuplicateUsername(const string& username);
int  findStaffIndex(const string& username);

bool login(const vector<Staff>& staffList, const string& username, const string& password);

void registerUser();
bool loginUser();

// Main entry point: displays Login/Register menu until login succeeds
void authenticationGate();

#endif