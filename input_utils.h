// Header Guard: prevents multiple inclusions
#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

/*
 ============================================================
  Input Validation Utilities
 ============================================================
  Provides robust input handling with fail-state recovery.
  Example: When cin >> int receives non-numeric input, cin enters
  a fail state; these functions properly clear and re-prompt.
 ============================================================
 */

#include <string>
#include <limits>
using namespace std;

// ============================================================
// Date Validation Functions
// ============================================================

bool isValidDate(const string& date);
string getValidatedDate(const string& prompt);

bool isValidFutureDate(const string& date);
string getValidatedFutureDate(const string& prompt);

bool isValidExpiryDate(const string& date);
string getValidatedExpiryDate(const string& prompt);

// ============================================================
// Integer Input (with range)
// ============================================================

int readValidatedInt(const string& prompt, int minVal, int maxVal);

// ============================================================
// Gender Input (M/F only)
// ============================================================

char readValidatedGender();

// ============================================================
// Non‑Empty Line Input
// ============================================================

string readNonEmptyLine(const string& prompt);

// ============================================================
// Double Input (with minimum value)
// ============================================================

double readValidatedDouble(const string& prompt, double minVal);

// ============================================================
// Phone Number Input (digits and hyphen, 9‑11 digits)
// ============================================================

string readValidatedPhone(const string& prompt);

// ============================================================
// Alphabetic‑Only Input (letters and spaces only)
// ============================================================

string readAlphaOnly(const string& prompt);

// ============================================================
// Generic Integer Input (no range)
// ============================================================

int getValidatedInt(const string& prompt);

// ============================================================
// Positive Integer Input (>0)
// ============================================================

int getPositiveInt(const string& prompt);

// ============================================================
// Non‑Negative Integer Input (>=0)
// ============================================================

int getNonNegativeInt(const string& prompt);

// ============================================================
// Generic Double Input
// ============================================================

double getValidatedDouble(const string& prompt);

// ============================================================
// Non‑Negative Double Input (>=0.0)
// ============================================================

double getNonNegativeDouble(const string& prompt);

// ============================================================
// Non‑Empty String Input
// ============================================================

string getNonEmptyString(const string& prompt);

// ============================================================
// Yes/No Input (Y/N)
// ============================================================

char getYesNo(const string& prompt);

// ============================================================
// Menu Choice Input (within range)
// ============================================================

int getValidatedMenuChoice(const string& prompt, int minOption, int maxOption);

// ============================================================
// UI Control Functions
// ============================================================

// Pause until user presses 0 (for viewing lists/reports)
void pressZeroToExit();

// Pause until user presses Enter (lighter, for operation feedback)
void waitForEnter();

// Clear terminal screen
void clearScreen();

#endif