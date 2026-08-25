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



#endif
