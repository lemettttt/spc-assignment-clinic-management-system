// #ifndef / #define / #endif 就是 Header Guard，用来避免同一个 declaration / definition 被处理多次。
#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H
/*
 ============================================================
  Input Validation Utilities
 ------------------------------------------------------------
  可以一起用的Input Validation工具
  Eg. 
  当cin>>int遇到num/符号这种非数字输入时，cin会进入"fail state"，如果没有正确清除，会导致程序stuck或无限loop
 ============================================================
 */

#include <string>
using namespace std;

// Date validation functions
bool isValidDate(const string& date);
string getValidatedDate(const string& prompt);

bool isValidFutureDate(const string& date);
string getValidatedFutureDate(const string& prompt);

bool isValidExpiryDate(const string& date);
string getValidatedExpiryDate(const string& prompt);



// Read an integer until the user enters a valid number (range can be specified)
// Eg：int choice = readValidatedInt("Enter your choice: ", 1, 6);
int readValidatedInt(const string& prompt, int minVal, int maxVal);


// Read gender, only accept m/M/f/F
char readValidatedGender();


// Read a line of text, disallowing empty input(such as enter only)
string readNonEmptyLine(const string& prompt);


// Read a double value
// Eg: double price = readValidatedDouble("Unit price: ", 0.0);
double readValidatedDouble(const string& prompt, double minVal);


// Read phone numbers: only numbers and "-" symbols are allowed, and the total number of numbers must be between 9-11 digits
string readValidatedPhone(const string& prompt);


// Read alphabetic-only input, not allowing numbers and symbols
string readAlphaOnly(const string& prompt);

// Read any integer from user input (no range restriction)
// Returns the integer entered by the user
int getValidatedInt(const string& prompt);

// Read a positive integer (greater than 0)
// Prompts until user enters a valid positive number
int getPositiveInt(const string& prompt);

// Read a non-negative integer (0 or greater)
// Prompts until user enters a valid non-negative number
int getNonNegativeInt(const string& prompt);

// Read any double from user input
// Returns the double value entered by the user
double getValidatedDouble(const string& prompt);

// Read a non-negative double (0.0 or greater)
// Prompts until user enters a valid non-negative number
double getNonNegativeDouble(const string& prompt);

// Read a non-empty string
// Rejects empty input and prompts until the user enters something
string getNonEmptyString(const string& prompt);

// Read a Yes/No response
// Accepts 'Y', 'y', 'N', 'n' and returns the uppercase character
char getYesNo(const string& prompt);

// Read a menu choice within a specified range
// Prompts until the user enters a valid option between minOption and maxOption
int getValidatedMenuChoice(const string& prompt, int minOption, int maxOption);

// Pause execution and wait for the user to press 0 to continue
// Used after displaying results so the user can read before the menu reappears
void pressZeroToContinue();                            

void clearScreen();

#endif
