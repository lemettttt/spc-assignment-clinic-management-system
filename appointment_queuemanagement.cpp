#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>
#include <limits>
 
#include "appointment_queuemanagement.h"
 
using namespace std;
 
// ============================================================
// CONSTANTS
// ============================================================
 
const int MAX_DOCTORS = 3;
const int MAX_SLOTS = 12;
 
// How many years ahead of today a booking is allowed
const int MAX_YEARS_AHEAD = 2;
 
// ============================================================
// DOCTOR INFORMATION
// ============================================================
 
int doctorIDs[MAX_DOCTORS] =
{
    101, 102, 103
};
 
string doctors[MAX_DOCTORS] =
{
    "Dr. Ahmad",
    "Dr. Lim",
    "Dr. Kumar"
};
 
// ============================================================
// FIXED TIME SLOTS
// ============================================================
 
string timeSlots[MAX_SLOTS] =
{
    "09:00 AM",
    "09:30 AM",
    "10:00 AM",
    "10:30 AM",
    "11:00 AM",
    "11:30 AM",
 
    "02:00 PM",
    "02:30 PM",
    "03:00 PM",
    "03:30 PM",
    "04:00 PM",
    "04:30 PM"
};
 
// ============================================================
// 2D ARRAY FOR DOCTOR AVAILABILITY
//
// true  = available
// false = unavailable
//
// Row 0 = Dr. Ahmad
// Row 1 = Dr. Lim
// Row 2 = Dr. Kumar
// ============================================================
 
bool doctorAvailability[MAX_DOCTORS][MAX_SLOTS] =
{
    // Dr. Ahmad
    {
        true, true, true, true, true, true,
        false, false, true, true, true, false
    },
 
    // Dr. Lim
    {
        false, false, true, true, true, false,
        true, true, true, true, false, false
    },
 
    // Dr. Kumar
    {
        true, true, true, false, false, false,
        true, true, true, false, false, false
    }
};
 
// ============================================================
// FUNCTION PROTOTYPES
// ============================================================
 
bool patientExists(
    const vector<Patient>& patients,
    int patientID
);
 
bool isValidDate(string date);
 
bool isDoctorAvailable(
    int doctorIndex,
    int slotIndex
);
 
bool checkSlotAvailability(
    const vector<Appointment>& appointments,
    int doctorID,
    string date,
    string timeSlot
);
 
int getDoctorIndex(int doctorID);
 
void waitForEnter();
 
bool isSlotInPast(
    const string& date,
    const string& timeSlot
);
 
int getValidatedInt(
    const string& prompt,
    int minValue,
    int maxValue
);
 
string getValidatedDate(const string& prompt);
 
int selectDoctor();
 
int generateNextAppointmentID(
    const vector<Appointment>& appointments
);
 
void bookAppointment(
    vector<Appointment>& appointments,
    vector<Patient>& patients
);
 
void displayDoctorSchedule(
    const vector<Appointment>& appointments
);
 
void cancelAppointment(
    vector<Appointment>& appointments
);
 
void rescheduleAppointment(
    vector<Appointment>& appointments
);
 
void manageQueue(
    vector<Appointment>& appointments,
    vector<Patient>& patients
);
 
void getNextPatientInQueue(
    vector<Appointment>& appointments
);
 
void displayTodayAppointments(
    const vector<Appointment>& appointments
);
 
 
// ============================================================
// CHECK WHETHER PATIENT EXISTS
// ============================================================
 
bool patientExists(
    const vector<Patient>& patients,
    int patientID)
{
    for (int i = 0; i < (int)patients.size(); i++)
    {
        if (patients[i].patientID == patientID)
        {
            return true;
        }
    }
 
    return false;
}
 
 
// ============================================================
// GET DOCTOR ARRAY INDEX
// ============================================================
 
int getDoctorIndex(int doctorID)
{
    for (int i = 0; i < MAX_DOCTORS; i++)
    {
        if (doctorIDs[i] == doctorID)
        {
            return i;
        }
    }
 
    return -1;
}
 
 
// ============================================================
// PAUSE UNTIL THE USER PRESSES ENTER
// Used after each action so results stay on screen until the
// user chooses to go back, instead of jumping to the menu.
// ============================================================
 
void waitForEnter()
{
    cout << "\nPress Enter to return to the menu...";
 
    // Discard anything left over from the previous input
    // (e.g. the newline after the last cin >> ...)
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
 
    // Now actually wait for the user to press Enter
    cin.get();
}
 
 
// ============================================================
// CHECK WHETHER A TIME SLOT HAS ALREADY PASSED
// Only relevant when the appointment date is today; any
// future date is never "in the past".
// ============================================================
 
bool isSlotInPast(
    const string& date,
    const string& timeSlot)
{
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
 
    int todayYear  = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay   = localTime->tm_mday;
 
    // Build today's date as DD/MM/YYYY (same format used
    // everywhere else) so it can be compared directly.
    ostringstream oss;
 
    oss << setfill('0') << setw(2) << todayDay  << "/"
        << setfill('0') << setw(2) << todayMonth << "/"
        << todayYear;
 
    // Not today -> can never be "in the past" here
    if (date != oss.str())
    {
        return false;
    }
 
    // Parse "hh:mm AM/PM" into minutes since midnight
    int hour   = stoi(timeSlot.substr(0, 2));
    int minute = stoi(timeSlot.substr(3, 2));
    string period = timeSlot.substr(6, 2);
 
    if (period == "PM" && hour != 12)
    {
        hour += 12;
    }
 
    if (period == "AM" && hour == 12)
    {
        hour = 0;
    }
 
    int slotMinutes = (hour * 60) + minute;
    int nowMinutes  = (localTime->tm_hour * 60) + localTime->tm_min;
 
    return slotMinutes <= nowMinutes;
}
 
 
// ============================================================
// GENERIC VALIDATED INTEGER INPUT
// Reused by every menu / choice / ID prompt so validation
// logic lives in exactly one place.
// ============================================================
 
int getValidatedInt(
    const string& prompt,
    int minValue,
    int maxValue)
{
    int value;
 
    cout << prompt;
 
    while (!(cin >> value) ||
           value < minValue ||
           value > maxValue)
    {
        cin.clear();
        cin.ignore(1000, '\n');
 
        cout << "Invalid input.\n";
        cout << "Please enter a number from "
             << minValue << " to " << maxValue << ": ";
    }
 
    return value;
}
 
 
// ============================================================
// DATE VALIDATION
// Format: DD/MM/YYYY
// Rejects malformed dates, impossible calendar dates,
// dates in the past, and dates too far in the future.
// ============================================================
 
bool isValidDate(string date)
{
    // Example: 25/08/2026 = 10 characters
    if (date.length() != 10)
    {
        return false;
    }
 
    // Check slash positions
    if (date[2] != '/' || date[5] != '/')
    {
        return false;
    }
 
    // Check all other characters are digits
    for (int i = 0; i < (int)date.length(); i++)
    {
        if (i != 2 && i != 5)
        {
            if (!isdigit((unsigned char)date[i]))
            {
                return false;
            }
        }
    }
 
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));
 
    if (month < 1 || month > 12)
    {
        return false;
    }
 
    int daysInMonth[12] =
    {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };
 
    // Leap year
    if ((year % 400 == 0) ||
        (year % 4 == 0 && year % 100 != 0))
    {
        daysInMonth[1] = 29;
    }
 
    if (day < 1 || day > daysInMonth[month - 1])
    {
        return false;
    }
 
    // --------------------------------------------------------
    // Compare against today's date (no more hardcoded years)
    // --------------------------------------------------------
 
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
 
    int todayYear  = localTime->tm_year + 1900;
    int todayMonth = localTime->tm_mon + 1;
    int todayDay   = localTime->tm_mday;
 
    // Reject dates before today
    if (year < todayYear ||
        (year == todayYear && month < todayMonth) ||
        (year == todayYear && month == todayMonth && day < todayDay))
    {
        return false;
    }
 
    // Reject dates too far in the future
    if (year > todayYear + MAX_YEARS_AHEAD)
    {
        return false;
    }
 
    return true;
}
 
 
// ============================================================
// PROMPT + VALIDATE A DATE
// Wraps isValidDate() in a retry loop so callers don't
// repeat the same while-loop everywhere.
// ============================================================
 
string getValidatedDate(const string& prompt)
{
    string date;
 
    cout << prompt;
    cin >> date;
 
    while (!isValidDate(date))
    {
        cout << "Invalid date. Dates must be real, ";
        cout << "not in the past, and within "
             << MAX_YEARS_AHEAD << " years from today.\n";
        cout << prompt;
 
        cin >> date;
    }
 
    return date;
}
 
 
// ============================================================
// PROMPT + VALIDATE A DOCTOR SELECTION
// Returns the doctor's array INDEX (0-based).
// ============================================================
 
int selectDoctor()
{
    cout << "\nAvailable Doctors:\n";
 
    for (int i = 0; i < MAX_DOCTORS; i++)
    {
        cout << i + 1 << ". "
             << doctors[i]
             << " (ID: " << doctorIDs[i] << ")\n";
    }
 
    int choice = getValidatedInt(
        "Select Doctor (1-" + to_string(MAX_DOCTORS) + "): ",
        1,
        MAX_DOCTORS);
 
    return choice - 1;
}
 
 
// ============================================================
// CHECK DOCTOR'S FIXED WORKING SCHEDULE
// ============================================================
 
bool isDoctorAvailable(
    int doctorIndex,
    int slotIndex)
{
    if (doctorIndex < 0 ||
        doctorIndex >= MAX_DOCTORS)
    {
        return false;
    }
 
    if (slotIndex < 0 ||
        slotIndex >= MAX_SLOTS)
    {
        return false;
    }
 
    return doctorAvailability[doctorIndex][slotIndex];
}
 
 
// ============================================================
// CHECK WHETHER A SLOT HAS ALREADY BEEN BOOKED
// ============================================================
 
bool checkSlotAvailability(
    const vector<Appointment>& appointments,
    int doctorID,
    string date,
    string timeSlot)
{
    for (int i = 0; i < (int)appointments.size(); i++)
    {
        if (appointments[i].doctorID == doctorID &&
            appointments[i].date == date &&
            appointments[i].timeSlot == timeSlot &&
            appointments[i].isCancelled == false)
        {
            return false;
        }
    }
 
    return true;
}
 
 
// ============================================================
// GENERATE NEXT APPOINTMENT ID
// Uses the max ID present rather than assuming the vector
// is always sorted / append-only.
// ============================================================
 
int generateNextAppointmentID(
    const vector<Appointment>& appointments)
{
    int maxID = 1000; // so the first ID issued is 1001
 
    for (int i = 0; i < (int)appointments.size(); i++)
    {
        if (appointments[i].appointmentID > maxID)
        {
            maxID = appointments[i].appointmentID;
        }
    }
 
    return maxID + 1;
}
 
 
// ============================================================
// APPOINTMENT & QUEUE MAIN MENU
// ============================================================
 
void appointmentQueueMenu(
    vector<Appointment>& appointments,
    vector<Patient>& patients)
{
    int choice;
 
    do
    {
        cout << "\n";
        cout << "=========================================\n";
        cout << "   APPOINTMENT & QUEUE MANAGEMENT\n";
        cout << "=========================================\n";
        cout << "1. Book Appointment\n";
        cout << "2. Display Doctor Schedule\n";
        cout << "3. Cancel Appointment\n";
        cout << "4. Reschedule Appointment\n";
        cout << "5. Manage Patient Queue\n";
        cout << "6. Call Next Patient\n";
        cout << "7. Display Today's Appointments\n";
        cout << "0. Back to Main Menu\n";
        cout << "=========================================\n";
 
        choice = getValidatedInt("Enter choice: ", 0, 7);
 
        switch (choice)
        {
        case 1:
            bookAppointment(appointments, patients);
            waitForEnter();
            break;
 
        case 2:
            displayDoctorSchedule(appointments);
            waitForEnter();
            break;
 
        case 3:
            cancelAppointment(appointments);
            waitForEnter();
            break;
 
        case 4:
            rescheduleAppointment(appointments);
            waitForEnter();
            break;
 
        case 5:
            manageQueue(appointments, patients);
            waitForEnter();
            break;
 
        case 6:
            getNextPatientInQueue(appointments);
            waitForEnter();
            break;
 
        case 7:
            displayTodayAppointments(appointments);
            waitForEnter();
            break;
 
        case 0:
            cout << "Returning to Main Menu...\n";
            break;
        }
 
    } while (choice != 0);
}
 
 
// ============================================================
// BOOK APPOINTMENT
// ============================================================
 
void bookAppointment(
    vector<Appointment>& appointments,
    vector<Patient>& patients)
{
    Appointment newAppointment;
 
    cout << "\n";
    cout << "=========================================\n";
    cout << "           BOOK APPOINTMENT\n";
    cout << "=========================================\n";
 
    // --------------------------------------------------------
    // Patient ID
    // --------------------------------------------------------
 
    newAppointment.patientID = getValidatedInt(
        "Enter Patient ID: ", 1, 999999999);
 
    if (!patientExists(
            patients,
            newAppointment.patientID))
    {
        cout << "\nPatient ID not found.\n";
        cout << "Please register the patient first.\n";
        return;
    }
 
    // --------------------------------------------------------
    // Doctor
    // --------------------------------------------------------
 
    int doctorIndex = selectDoctor();
 
    newAppointment.doctorID =
        doctorIDs[doctorIndex];
 
    // --------------------------------------------------------
    // Date
    // --------------------------------------------------------
 
    newAppointment.date = getValidatedDate(
        "\nEnter Appointment Date (DD/MM/YYYY): ");
 
    // --------------------------------------------------------
    // Show only this doctor's WORKING slots, renumbered 1..N
    // so the list stays short and matches the select prompt
    // --------------------------------------------------------
 
    cout << "\nAvailable Time Slots for "
         << doctors[doctorIndex] << ":\n\n";
 
    cout << left
         << setw(5)  << "No."
         << setw(12) << "Time"
         << setw(12) << "Status"
         << endl;
 
    cout << "-----------------------------\n";
 
    // slotIndexMap remembers which REAL slot (0-11) each
    // displayed number (1, 2, 3...) points back to.
    // slotCount = how many working slots we found.
    int slotIndexMap[MAX_SLOTS];
    int slotCount = 0;
 
    for (int i = 0; i < MAX_SLOTS; i++)
    {
        if (!isDoctorAvailable(doctorIndex, i))
        {
            continue;
        }
 
        slotIndexMap[slotCount] = i;
        slotCount++;
 
        cout << left
             << setw(5)  << slotCount
             << setw(12) << timeSlots[i];
 
        if (isSlotInPast(newAppointment.date, timeSlots[i]))
        {
            cout << "Passed";
        }
        else if (!checkSlotAvailability(
                     appointments,
                     newAppointment.doctorID,
                     newAppointment.date,
                     timeSlots[i]))
        {
            cout << "Booked";
        }
        else
        {
            cout << "Available";
        }
 
        cout << endl;
    }
 
    if (slotCount == 0)
    {
        cout << "\nThis doctor has no working slots.\n";
        return;
    }
 
    // --------------------------------------------------------
    // Select Time
    // --------------------------------------------------------
 
    int slotChoice = getValidatedInt(
        "\nSelect Time Slot (1-" + to_string(slotCount) + "): ",
        1,
        slotCount);
 
    int slotIndex = slotIndexMap[slotChoice - 1];
 
    // --------------------------------------------------------
    // Check the slot hasn't already passed (today only)
    // --------------------------------------------------------
 
    if (isSlotInPast(
            newAppointment.date,
            timeSlots[slotIndex]))
    {
        cout << "\n";
        cout << timeSlots[slotIndex]
             << " has already passed for today.\n";
 
        cout << "Please select a later time slot.\n";
        return;
    }
 
    newAppointment.timeSlot =
        timeSlots[slotIndex];
 
    // --------------------------------------------------------
    // Check double booking
    // --------------------------------------------------------
 
    if (!checkSlotAvailability(
            appointments,
            newAppointment.doctorID,
            newAppointment.date,
            newAppointment.timeSlot))
    {
        cout << "\n";
        cout << "This time slot has already been booked.\n";
        cout << "Please select another time slot.\n";
        return;
    }
 
    // --------------------------------------------------------
    // Generate Appointment ID
    // --------------------------------------------------------
 
    newAppointment.appointmentID =
        generateNextAppointmentID(appointments);
 
    newAppointment.isCancelled = false;
    newAppointment.isServed = false;
 
    // Save appointment
    appointments.push_back(newAppointment);
 
    // --------------------------------------------------------
    // Confirmation
    // --------------------------------------------------------
 
    cout << "\n";
    cout << "=========================================\n";
    cout << "     APPOINTMENT BOOKED SUCCESSFULLY\n";
    cout << "=========================================\n";
 
    cout << left
         << setw(20) << "Appointment ID:"
         << newAppointment.appointmentID << endl;
 
    cout << left
         << setw(20) << "Patient ID:"
         << newAppointment.patientID << endl;
 
    cout << left
         << setw(20) << "Doctor:"
         << doctors[doctorIndex] << endl;
 
    cout << left
         << setw(20) << "Date:"
         << newAppointment.date << endl;
 
    cout << left
         << setw(20) << "Time:"
         << newAppointment.timeSlot << endl;
 
    cout << left
         << setw(20) << "Status:"
         << "Booked" << endl;
 
    cout << "=========================================\n";
}
 
 
// ============================================================
// DISPLAY DOCTOR SCHEDULE
// ============================================================
 
void displayDoctorSchedule(
    const vector<Appointment>& appointments)
{
    cout << "\n";
    cout << "=========================================\n";
    cout << "        DISPLAY DOCTOR SCHEDULE\n";
    cout << "=========================================\n";
 
    int doctorIndex = selectDoctor();
 
    string date = getValidatedDate("Enter Date (DD/MM/YYYY): ");
 
    cout << "\n";
    cout << "Doctor : " << doctors[doctorIndex] << endl;
    cout << "Date   : " << date << endl;
 
    cout << "\n";
    cout << left
         << setw(5)  << "No."
         << setw(12) << "Time"
         << setw(12) << "Status"
         << endl;
 
    cout << "-----------------------------\n";
 
    for (int i = 0; i < MAX_SLOTS; i++)
    {
        cout << left
             << setw(5)  << i + 1
             << setw(12) << timeSlots[i];
 
        // Doctor doesn't work at this time
        if (!isDoctorAvailable(
                doctorIndex,
                i))
        {
            cout << "Unavailable";
        }
        else if (isSlotInPast(date, timeSlots[i]))
        {
            cout << "Passed";
        }
        else
        {
            bool booked =
                !checkSlotAvailability(
                    appointments,
                    doctorIDs[doctorIndex],
                    date,
                    timeSlots[i]);
 
            if (booked)
            {
                cout << "Booked";
            }
            else
            {
                cout << "Available";
            }
        }
 
        cout << endl;
    }
}
 
 
// ============================================================
// CANCEL APPOINTMENT
// ============================================================
 
void cancelAppointment(
    vector<Appointment>& appointments)
{
    cout << "\n";
    cout << "=========================================\n";
    cout << "          CANCEL APPOINTMENT\n";
    cout << "=========================================\n";
 
    int appointmentID = getValidatedInt(
        "Enter Appointment ID: ", 1, 999999999);
 
    for (int i = 0; i < (int)appointments.size(); i++)
    {
        if (appointments[i].appointmentID ==
            appointmentID)
        {
            if (appointments[i].isCancelled)
            {
                cout << "This appointment is already cancelled.\n";
                return;
            }
 
            if (appointments[i].isServed)
            {
                cout << "This appointment has already been served ";
                cout << "and cannot be cancelled.\n";
                return;
            }
 
            cout << "\nAppointment Found\n";
 
            cout << "Patient ID : "
                 << appointments[i].patientID << endl;
 
            cout << "Doctor ID  : "
                 << appointments[i].doctorID << endl;
 
            cout << "Date       : "
                 << appointments[i].date << endl;
 
            cout << "Time       : "
                 << appointments[i].timeSlot << endl;
 
            cout << "\nAre you sure you want to cancel?\n";
            cout << "1. Yes\n";
            cout << "2. No\n";
 
            int confirm = getValidatedInt("Enter choice: ", 1, 2);
 
            if (confirm == 1)
            {
                appointments[i].isCancelled = true;
 
                cout << "\nAppointment cancelled successfully.\n";
            }
            else
            {
                cout << "\nAppointment cancellation aborted.\n";
            }
 
            return;
        }
    }
 
    cout << "Appointment ID not found.\n";
}
 
 
// ============================================================
// RESCHEDULE APPOINTMENT
// ============================================================
 
void rescheduleAppointment(
    vector<Appointment>& appointments)
{
    cout << "\n";
    cout << "=========================================\n";
    cout << "         RESCHEDULE APPOINTMENT\n";
    cout << "=========================================\n";
 
    int appointmentID = getValidatedInt(
        "Enter Appointment ID: ", 1, 999999999);
 
    for (int i = 0; i < (int)appointments.size(); i++)
    {
        if (appointments[i].appointmentID ==
            appointmentID)
        {
            if (appointments[i].isCancelled)
            {
                cout << "This appointment has already been cancelled.\n";
                return;
            }
 
            if (appointments[i].isServed)
            {
                cout << "This appointment has already been served ";
                cout << "and cannot be rescheduled.\n";
                return;
            }
 
            int doctorIndex =
                getDoctorIndex(
                    appointments[i].doctorID);
 
            if (doctorIndex == -1)
            {
                cout << "\nError: Doctor record not found for this appointment.\n";
                return;
            }
 
            cout << "\nCurrent Appointment\n";
 
            cout << "Patient ID : "
                 << appointments[i].patientID << endl;
 
            cout << "Doctor     : "
                 << doctors[doctorIndex] << endl;
 
            cout << "Date       : "
                 << appointments[i].date << endl;
 
            cout << "Time       : "
                 << appointments[i].timeSlot << endl;
 
            // ------------------------------------------------
            // New Date
            // ------------------------------------------------
 
            string newDate = getValidatedDate(
                "\nEnter New Date (DD/MM/YYYY): ");
 
            // ------------------------------------------------
            // Show only WORKING slots, renumbered 1..N
            // ------------------------------------------------
 
            cout << "\nAvailable Time Slots:\n\n";
 
            cout << left
                 << setw(5)  << "No."
                 << setw(12) << "Time"
                 << setw(12) << "Status"
                 << endl;
 
            cout << "-----------------------------\n";
 
            // slotIndexMap remembers which REAL slot (0-11)
            // each displayed number points back to.
            int slotIndexMap[MAX_SLOTS];
            int slotCount = 0;
 
            for (int j = 0; j < MAX_SLOTS; j++)
            {
                if (!isDoctorAvailable(doctorIndex, j))
                {
                    continue;
                }
 
                slotIndexMap[slotCount] = j;
                slotCount++;
 
                cout << left
                     << setw(5)  << slotCount
                     << setw(12) << timeSlots[j];
 
                if (isSlotInPast(newDate, timeSlots[j]))
                {
                    cout << "Passed";
                }
                else
                {
                    bool booked = false;
 
                    for (int k = 0;
                         k < (int)appointments.size();
                         k++)
                    {
                        // Don't compare appointment
                        // against itself
                        if (k == i)
                        {
                            continue;
                        }
 
                        if (appointments[k].doctorID ==
                                appointments[i].doctorID &&
                            appointments[k].date ==
                                newDate &&
                            appointments[k].timeSlot ==
                                timeSlots[j] &&
                            appointments[k].isCancelled == false)
                        {
                            booked = true;
                            break;
                        }
                    }
 
                    if (booked)
                    {
                        cout << "Booked";
                    }
                    else
                    {
                        cout << "Available";
                    }
                }
 
                cout << endl;
            }
 
            if (slotCount == 0)
            {
                cout << "\nThis doctor has no working slots.\n";
                return;
            }
 
            // ------------------------------------------------
            // Select new slot
            // ------------------------------------------------
 
            int newSlot = getValidatedInt(
                "\nSelect New Time Slot (1-" +
                    to_string(slotCount) + "): ",
                1,
                slotCount);
 
            int newSlotIndex = slotIndexMap[newSlot - 1];
 
            if (isSlotInPast(newDate, timeSlots[newSlotIndex]))
            {
                cout << "\n" << timeSlots[newSlotIndex]
                     << " has already passed for today.\n";
 
                cout << "Please select a later time slot.\n";
                return;
            }
 
            string newTime =
                timeSlots[newSlotIndex];
 
            // Check double booking
            for (int j = 0;
                 j < (int)appointments.size();
                 j++)
            {
                if (j == i)
                {
                    continue;
                }
 
                if (appointments[j].doctorID ==
                        appointments[i].doctorID &&
                    appointments[j].date ==
                        newDate &&
                    appointments[j].timeSlot ==
                        newTime &&
                    appointments[j].isCancelled == false)
                {
                    cout << "\nThe selected slot is already booked.\n";
                    return;
                }
            }
 
            // Update appointment
            appointments[i].date = newDate;
            appointments[i].timeSlot = newTime;
 
            cout << "\n";
            cout << "=========================================\n";
            cout << "     APPOINTMENT RESCHEDULED\n";
            cout << "=========================================\n";
 
            cout << "Appointment ID : "
                 << appointments[i].appointmentID << endl;
 
            cout << "New Date       : "
                 << newDate << endl;
 
            cout << "New Time       : "
                 << newTime << endl;
 
            cout << "=========================================\n";
 
            return;
        }
    }
 
    cout << "Appointment ID not found.\n";
}
 
 
// ============================================================
// MANAGE QUEUE
// ============================================================
 
void manageQueue(
    vector<Appointment>& appointments,
    vector<Patient>& patients)
{
    cout << "\n";
    cout << "=========================================\n";
    cout << "          PATIENT QUEUE\n";
    cout << "=========================================\n";
 
    string date = getValidatedDate("Enter Date (DD/MM/YYYY): ");
 
    int doctorIndex = selectDoctor();
    int doctorID = doctorIDs[doctorIndex];
 
    cout << "\n";
    cout << "Queue for "
         << doctors[doctorIndex]
         << " on " << date << endl;
 
    cout << "-------------------------------------------------------\n";
 
    cout << left
         << setw(10) << "Queue"
         << setw(15) << "Patient ID"
         << setw(15) << "Time"
         << setw(15) << "Appointment"
         << endl;
 
    cout << "-------------------------------------------------------\n";
 
    int queueNumber = 1;
 
    // Go through time slots in chronological order
    for (int slot = 0; slot < MAX_SLOTS; slot++)
    {
        for (int i = 0;
             i < (int)appointments.size();
             i++)
        {
            if (appointments[i].doctorID == doctorID &&
                appointments[i].date == date &&
                appointments[i].timeSlot == timeSlots[slot] &&
                appointments[i].isCancelled == false &&
                appointments[i].isServed == false)
            {
                cout << left
                     << setw(10)
                     << queueNumber
 
                     << setw(15)
                     << appointments[i].patientID
 
                     << setw(15)
                     << appointments[i].timeSlot
 
                     << setw(15)
                     << appointments[i].appointmentID
 
                     << endl;
 
                queueNumber++;
            }
        }
    }
 
    if (queueNumber == 1)
    {
        cout << "No patients are currently waiting.\n";
    }
}
 
 
// ============================================================
// CALL NEXT PATIENT
// ============================================================
 
void getNextPatientInQueue(
    vector<Appointment>& appointments)
{
    cout << "\n";
    cout << "=========================================\n";
    cout << "           CALL NEXT PATIENT\n";
    cout << "=========================================\n";
 
    string date = getValidatedDate("Enter Date (DD/MM/YYYY): ");
 
    int doctorIndex = selectDoctor();
    int doctorID = doctorIDs[doctorIndex];
 
    // Find earliest appointment
    for (int slot = 0;
         slot < MAX_SLOTS;
         slot++)
    {
        for (int i = 0;
             i < (int)appointments.size();
             i++)
        {
            if (appointments[i].doctorID == doctorID &&
                appointments[i].date == date &&
                appointments[i].timeSlot == timeSlots[slot] &&
                appointments[i].isCancelled == false &&
                appointments[i].isServed == false)
            {
                appointments[i].isServed = true;
 
                cout << "\n";
                cout << "=========================================\n";
                cout << "             NEXT PATIENT\n";
                cout << "=========================================\n";
 
                cout << "Patient ID     : "
                     << appointments[i].patientID << endl;
 
                cout << "Appointment ID : "
                     << appointments[i].appointmentID << endl;
 
                cout << "Time           : "
                     << appointments[i].timeSlot << endl;
 
                cout << "Status         : Now being served\n";
 
                cout << "=========================================\n";
 
                return;
            }
        }
    }
 
    cout << "\nNo patients are waiting for this doctor.\n";
}
 
 
// ============================================================
// DISPLAY APPOINTMENTS BY DATE
// ============================================================
 
void displayTodayAppointments(
    const vector<Appointment>& appointments)
{
    cout << "\n";
    cout << "=========================================\n";
    cout << "       TODAY'S APPOINTMENTS\n";
    cout << "=========================================\n";
 
    string date = getValidatedDate("Enter Date (DD/MM/YYYY): ");
 
    cout << "\nAppointments on "
         << date << endl;
 
    cout << "---------------------------------------------------------------------\n";
 
    cout << left
         << setw(15) << "Appointment"
         << setw(12) << "Patient"
         << setw(15) << "Doctor"
         << setw(15) << "Time"
         << setw(12) << "Status"
         << endl;
 
    cout << "---------------------------------------------------------------------\n";
 
    bool found = false;
 
    for (int i = 0;
         i < (int)appointments.size();
         i++)
    {
        if (appointments[i].date == date)
        {
            found = true;
 
            int doctorIndex =
                getDoctorIndex(
                    appointments[i].doctorID);
 
            string doctorName =
                (doctorIndex != -1) ? doctors[doctorIndex] : "Unknown";
 
            cout << left
                 << setw(15)
                 << appointments[i].appointmentID
 
                 << setw(12)
                 << appointments[i].patientID
 
                 << setw(15)
                 << doctorName
 
                 << setw(15)
                 << appointments[i].timeSlot;
 
            if (appointments[i].isCancelled)
            {
                cout << setw(12)
                     << "Cancelled";
            }
            else if (appointments[i].isServed)
            {
                cout << setw(12)
                     << "Served";
            }
            else
            {
                cout << setw(12)
                     << "Booked";
            }
 
            cout << endl;
        }
    }
 
    if (!found)
    {
        cout << "No appointments found for this date.\n";
    }
}