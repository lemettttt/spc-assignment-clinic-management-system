#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <iostream>
#include <string>
#include <limits>
#include <cctype>

using namespace std;


// ============================================================
// VALIDATE INTEGER INPUT
// ============================================================

inline int getValidatedInt(string prompt) {
    int value;

    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid input. Please enter a valid number.\n";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}


// ============================================================
// VALIDATE POSITIVE INTEGER (> 0)
// ============================================================

inline int getPositiveInt(string prompt) {
    int value;

    while (true) {
        value = getValidatedInt(prompt);

        if (value > 0) {
            return value;
        }

        cout << "Invalid input. Please enter a number greater than 0.\n";
    }
}


// ============================================================
// VALIDATE NON-NEGATIVE INTEGER (>= 0)
// ============================================================

inline int getNonNegativeInt(string prompt) {
    int value;

    while (true) {
        value = getValidatedInt(prompt);

        if (value >= 0) {
            return value;
        }

        cout << "Invalid input. Please enter 0 or a positive number.\n";
    }
}


// ============================================================
// VALIDATE DOUBLE INPUT
// ============================================================

inline double getValidatedDouble(string prompt) {
    double value;

    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid input. Please enter a valid number.\n";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}


// ============================================================
// VALIDATE POSITIVE DOUBLE
// ============================================================

inline double getNonNegativeDouble(string prompt) {
    double value;

    while (true) {
        value = getValidatedDouble(prompt);

        if (value >= 0) {
            return value;
        }

        cout << "Invalid input. Value cannot be negative.\n";
    }
}


// ============================================================
// VALIDATE NON-EMPTY STRING
// ============================================================

inline string getNonEmptyString(string prompt) {
    string value;

    while (true) {
        cout << prompt;
        getline(cin, value);

        if (!value.empty()) {
            return value;
        }

        cout << "Invalid input. This field cannot be empty.\n";
    }
}


// ============================================================
// VALIDATE YES / NO INPUT
// ============================================================

inline char getYesNo(string prompt) {
    char answer;

    while (true) {
        cout << prompt;
        cin >> answer;

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        answer = toupper(
            static_cast<unsigned char>(answer)
        );

        if (answer == 'Y' || answer == 'N') {
            return answer;
        }

        cout << "Invalid input. Please enter Y or N.\n";
    }
}


// ============================================================
// VALIDATE MENU CHOICE
// ============================================================

inline int getValidatedMenuChoice(
    string prompt,
    int minOption,
    int maxOption
) {
    int choice;

    while (true) {
        choice = getValidatedInt(prompt);

        if (choice >= minOption &&
            choice <= maxOption) {

            return choice;
        }

        cout << "Invalid choice. Please enter a number between "
             << minOption
             << " and "
             << maxOption
             << ".\n";
    }
}

#endif