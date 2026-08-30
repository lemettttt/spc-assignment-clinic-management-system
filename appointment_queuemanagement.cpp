#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <ctime>
#include <limits>
 
#include "appointment_queuemanagement.h"
#include "input_utils.h"
 
using namespace std;
 
// ============================================================
// CONSTANTS
// ============================================================
//
// MAX_DOCTORS, doctorIDs[], and MAX_SLOTS come from
// clinic_shared.h now — do NOT redeclare them here, that
// would conflict with the shared definitions in
// clinic_shared.cpp (duplicate/multiple-definition errors).
 
const int MAX_SLOTS = 12;
 
// How many years ahead of today a booking is allowed
const int MAX_YEARS_AHEAD = 2;
 
// ============================================================
// FIXED TIME SLOTS
// (doctor names/IDs now come from clinic_shared.h's
// doctorNames[] / doctorIDs[] — this module only owns the
// appointment-slot-specific data)
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
 
string getTodayDateString();
 
bool isSlotInPast(
    const string& date,
    const string& timeSlot
);
 
int getValidatedInt(
    const string& prompt,
    int minValue,
    int maxValue
);
 
string getValidatedAppointmentDate(const string& prompt);

bool isDoctorClinicClosedForToday(int doctorIndex);
 
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
// GET TODAY'S DATE AS DD/MM/YYYY
// (same format used everywhere else). Needed by isSlotInPast()
// and by the real-time queue functions below.
// ============================================================
 
string getTodayDateString()
{
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
 
    int year  = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day   = localTime->tm_mday;
 
    ostringstream oss;
 
    oss << setfill('0') << setw(2) << day  << "/"
        << setfill('0') << setw(2) << month << "/"
        << year;
 
    return oss.str();
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
    // Not today -> can never be "in the past" here
    if (date != getTodayDateString())
    {
        return false;
    }
 
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
 
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
// 判断医生今天的门诊是否已经结束
// 遍历医生所有可用的 Slot，找出最晚的一个时间并和当前时间比较
// ============================================================

bool isDoctorClinicClosedForToday(int doctorIndex) {
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);

    int nowMinutes = (localTime->tm_hour * 60) + localTime->tm_min;

    int latestSlotMinutes = -1;

    for (int i = 0; i < MAX_SLOTS; i++) {
        if (!isDoctorAvailable(doctorIndex, i)) {
            continue; // 跳过医生不工作的时间段
        }

        string timeSlot = timeSlots[i];
        int hour   = stoi(timeSlot.substr(0, 2));
        int minute = stoi(timeSlot.substr(3, 2));
        string period = timeSlot.substr(6, 2);

        if (period == "PM" && hour != 12) hour += 12;
        if (period == "AM" && hour == 12) hour = 0;

        int slotMinutes = (hour * 60) + minute;

        if (slotMinutes > latestSlotMinutes) {
            latestSlotMinutes = slotMinutes;
        }
    }

    // 如果当前时间 > 最后一个时间段的开始时间，说明已经结束了
    if (latestSlotMinutes != -1 && nowMinutes >= latestSlotMinutes) {
        return true;
    }

    return false;
}

string getValidatedAppointmentDate(const string& prompt) {
    string date;
    
    while (true) {

        // 【修复1】清除上一次 cin>> 残留的换行符，防止 getline 读取到空行
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // 直接使用队友写好的函数（负责格式校验和过去日期校验）
        date = getValidatedFutureDate(prompt); 

        // 实现你的 2年限制 业务逻辑
        time_t rawTime = time(nullptr);
        tm* localTime = localtime(&rawTime);

        int todayYear = localTime->tm_year + 1900;
        int inputYear = stoi(date.substr(6, 4));

        if (inputYear <= todayYear + MAX_YEARS_AHEAD) {
            return date; // 有效，在2年以内
        }

        cout << "Error: Date cannot be more than " 
             << MAX_YEARS_AHEAD << " years in the future.\n";
    }
}
 
 
// ============================================================
// PROMPT + VALIDATE A DOCTOR SELECTION
// Returns the doctor's array INDEX (0-based), or -1 if the
// user chose to cancel.
// ============================================================
 
int selectDoctor()
{
    cout << "\nAvailable Doctors:\n";
    cout << "0. Cancel / Back\n";
 
    for (int i = 0; i < MAX_DOCTORS; i++)
    {
        cout << i + 1 << ". "
             << doctorNames[i]
             << " (ID: " << doctorIDs[i] << ")\n";
    }
 
    int choice = getValidatedInt(
        "Select Doctor (0 to cancel, 1-" +
            to_string(MAX_DOCTORS) + "): ",
        0,
        MAX_DOCTORS);
 
    if (choice == 0)
    {
        return -1; // signals "user cancelled"
    }
 
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
            appointments[i].appointmentDate == date &&
            appointments[i].timeSlot == timeSlot &&
            appointments[i].status != "Cancelled")
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
 
void appointmentQueueMenu(vector<Appointment>& appointments, vector<Patient>& patients) {
    int choice;

    do {
        clearScreen();
        
        setColor(COLOR_DARK_BLUE);
        cout << "\n  ============================================================\n";
        cout << "  |            APPOINTMENT & QUEUE MANAGEMENT                |\n";
        cout << "  +----------------------------------------------------------+\n";
        cout << "  |                                                          |\n";
        
        setColor(COLOR_LIGHT_BLUE);
        cout << "  |   1.  Book Appointment                                   |\n";
        cout << "  |   2.  Display Doctor Schedule                            |\n";
        cout << "  |   3.  Cancel Appointment                                 |\n";
        cout << "  |   4.  Reschedule Appointment                             |\n";
        cout << "  |   5.  Manage Patient Queue                               |\n";
        cout << "  |   6.  Call Next Patient                                  |\n";
        cout << "  |   7.  Display Today's Appointments                       |\n";
        
        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        
        setColor(COLOR_RED);
        cout << "  |   0.  Back to Main Menu                                  |\n";
        
        setColor(COLOR_DARK_BLUE);
        cout << "  |                                                          |\n";
        cout << "  ============================================================\n";
        setColor(COLOR_RESET);

        choice = getValidatedInt("  Enter your choice: ", 0, 7);

        switch (choice) {
            case 1: bookAppointment(appointments, patients); waitForEnter(); break;
            case 2: displayDoctorSchedule(appointments); waitForEnter(); break;
            case 3: cancelAppointment(appointments); waitForEnter(); break;
            case 4: rescheduleAppointment(appointments); waitForEnter(); break;
            case 5: manageQueue(appointments, patients); waitForEnter(); break;
            case 6: getNextPatientInQueue(appointments); waitForEnter(); break;
            case 7: displayTodayAppointments(appointments); waitForEnter(); break;
            case 0: cout << "\nReturning to Main Menu...\n"; break;
            default: cout << "\n[ERROR] Invalid choice.\n";
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
        "Enter Patient ID (0 to cancel): ", 0, 999999999);
 
    if (newAppointment.patientID == 0)
    {
        cout << "\nBooking cancelled.\n";
        return;
    }
 
    if (!patientExists(
            patients,
            newAppointment.patientID))
    {
        cout << "\nPatient ID not found.\n";
        cout << "Please register the patient first.\n";
        return;
    }

    // --------------------------------------------------------
    // Date
    // --------------------------------------------------------
 
    newAppointment.appointmentDate = getValidatedAppointmentDate(
        "\nEnter Appointment Date (DD/MM/YYYY): ");
 
    // --------------------------------------------------------
    // Doctor
    // --------------------------------------------------------
 
    int doctorIndex = selectDoctor();
 
    if (doctorIndex == -1)
    {
        cout << "\nBooking cancelled.\n";
        return;
    }
 
    newAppointment.doctorID =
        doctorIDs[doctorIndex];
    
         // ============================================================
    // 【修复】判断今天是否已经下班
    // ============================================================
    if (newAppointment.appointmentDate == getTodayDateString() && 
        isDoctorClinicClosedForToday(doctorIndex)) {
        
        cout << "\n[System] " << doctorNames[doctorIndex] 
             << " 's clinic hours have ended for today.\n";
        cout << "[System] Please select tomorrow or a later date.\n";

        // 让用户重新输入日期，或者直接取消。 
        // 这里采用重新输入日期的逻辑：
        newAppointment.appointmentDate = getValidatedAppointmentDate(
            "\nPlease enter again the appointment date (DD/MM/YYYY): ");

        // 这里要注意，如果重新输入的日期依然是今天，会无限循环。
        // 为了彻底解决，我们写一个简单的 while 循环：
        while (newAppointment.appointmentDate == getTodayDateString() && 
               isDoctorClinicClosedForToday(doctorIndex)) {
            
            cout << "\n[System] You entered today's date, but the doctor's clinic hours have already ended.\n";
            newAppointment.appointmentDate = getValidatedAppointmentDate(
                "\nPlease enter again the appointment date (DD/MM/YYYY): ");
        }
    }
 
    // --------------------------------------------------------
    // Show only this doctor's WORKING slots, renumbered 1..N
    // so the list stays short and matches the select prompt
    // --------------------------------------------------------
 
    cout << "\nAvailable Time Slots for "
         << doctorNames[doctorIndex] << ":\n\n";
 
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
 
        if (isSlotInPast(newAppointment.appointmentDate, timeSlots[i]))
        {
            cout << "Passed";
        }
        else if (!checkSlotAvailability(
                     appointments,
                     newAppointment.doctorID,
                     newAppointment.appointmentDate,
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
    // Loop until the user picks a slot that is genuinely
    // free (not past, not already booked), or cancels with 0.
    // This is the fix for the old bug where picking a slot
    // that turned out to be unavailable would silently abort
    // the ENTIRE booking instead of asking again.
    // --------------------------------------------------------
 
    int slotIndex = -1;
 
    while (true)
    {
        int slotChoice = getValidatedInt(
            "\nSelect Time Slot (0 to cancel, 1-" +
                to_string(slotCount) + "): ",
            0,
            slotCount);
 
        if (slotChoice == 0)
        {
            cout << "\nBooking cancelled.\n";
            return;
        }
 
        int candidateIndex = slotIndexMap[slotChoice - 1];
 
        if (isSlotInPast(newAppointment.appointmentDate, timeSlots[candidateIndex]))
        {
            cout << "\n" << timeSlots[candidateIndex]
                 << " has already passed for today.\n";
            cout << "Please choose a different slot from the list above.\n";
            continue; // ask again instead of exiting
        }
 
        if (!checkSlotAvailability(
                appointments,
                newAppointment.doctorID,
                newAppointment.appointmentDate,
                timeSlots[candidateIndex]))
        {
            cout << "\n" << timeSlots[candidateIndex]
                 << " has already been booked.\n";
            cout << "Please choose a different slot from the list above.\n";
            continue; // ask again instead of exiting
        }
 
        // Slot is valid - stop looping
        slotIndex = candidateIndex;
        break;
    }
 
    newAppointment.timeSlot =
        timeSlots[slotIndex];
 
    // --------------------------------------------------------
    // Generate Appointment ID
    // --------------------------------------------------------
 
    newAppointment.appointmentID =
        generateNextAppointmentID(appointments);
 
    // doctorType comes from the shared "doctors" list (loaded via
    // initDoctorData() in clinic_shared.cpp) - fallback to "General"
    // if this doctor somehow isn't in that list yet.
    newAppointment.doctorType = "General";
 
    for (int i = 0; i < (int)doctors.size(); i++)
    {
        if (doctors[i].doctorID == newAppointment.doctorID)
        {
            newAppointment.doctorType = doctors[i].specialization;
            break;
        }
    }
 
    newAppointment.status = "Pending";
 
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
         << doctorNames[doctorIndex] << endl;
 
    cout << left
         << setw(20) << "Date:"
         << newAppointment.appointmentDate << endl;
 
    cout << left
         << setw(20) << "Time:"
         << newAppointment.timeSlot << endl;
 
    cout << left
         << setw(20) << "Status:"
         << newAppointment.status << endl;
 
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
 
    if (doctorIndex == -1)
    {
        cout << "\nCancelled.\n";
        return;
    }
 
    string date = getValidatedAppointmentDate("Enter Date (DD/MM/YYYY): ");

    if (date == getTodayDateString() && isDoctorClinicClosedForToday(doctorIndex)) {
        cout << "\n[System] " << doctorNames[doctorIndex] 
             << "'s clinic hours has ended for today. Cannot display today's schedule. \n";
        return;
    }
 
    cout << "\n";
    cout << "Doctor : " << doctorNames[doctorIndex] << endl;
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
        "Enter Appointment ID (0 to cancel): ", 0, 999999999);
 
    if (appointmentID == 0)
    {
        cout << "\nCancelled.\n";
        return;
    }
 
    for (int i = 0; i < (int)appointments.size(); i++)
    {
        if (appointments[i].appointmentID ==
            appointmentID)
        {
            if (appointments[i].status == "Cancelled")
            {
                cout << "This appointment is already cancelled.\n";
                return;
            }
 
            if (appointments[i].status == "Completed")
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
                 << appointments[i].appointmentDate << endl;
 
            cout << "Time       : "
                 << appointments[i].timeSlot << endl;
 
            cout << "\nAre you sure you want to cancel?\n";
            cout << "1. Yes\n";
            cout << "2. No\n";
 
            int confirm = getValidatedInt("Enter choice: ", 1, 2);
 
            if (confirm == 1)
            {
                appointments[i].status = "Cancelled";
 
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
        "Enter Appointment ID (0 to cancel): ", 0, 999999999);
 
    if (appointmentID == 0)
    {
        cout << "\nCancelled.\n";
        return;
    }
 
    for (int i = 0; i < (int)appointments.size(); i++)
    {
        if (appointments[i].appointmentID ==
            appointmentID)
        {
            if (appointments[i].status == "Cancelled")
            {
                cout << "This appointment has already been cancelled.\n";
                return;
            }
 
            if (appointments[i].status == "Completed")
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
                 << doctorNames[doctorIndex] << endl;
 
            cout << "Date       : "
                 << appointments[i].appointmentDate << endl;
 
            cout << "Time       : "
                 << appointments[i].timeSlot << endl;
 
            // ------------------------------------------------
            // New Date
            // ------------------------------------------------
 
            string newDate = getValidatedAppointmentDate(
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
                            appointments[k].appointmentDate ==
                                newDate &&
                            appointments[k].timeSlot ==
                                timeSlots[j] &&
                            appointments[k].status != "Cancelled")
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
            // Loop until a genuinely free slot is chosen, or
            // the user cancels with 0 — same fix as booking.
            // ------------------------------------------------
 
            int newSlotIndex = -1;
 
            while (true)
            {
                int newSlot = getValidatedInt(
                    "\nSelect New Time Slot (0 to cancel, 1-" +
                        to_string(slotCount) + "): ",
                    0,
                    slotCount);
 
                if (newSlot == 0)
                {
                    cout << "\nReschedule cancelled.\n";
                    return;
                }
 
                int candidateIndex = slotIndexMap[newSlot - 1];
 
                if (isSlotInPast(newDate, timeSlots[candidateIndex]))
                {
                    cout << "\n" << timeSlots[candidateIndex]
                         << " has already passed for today.\n";
                    cout << "Please choose a different slot from the list above.\n";
                    continue;
                }
 
                bool booked = false;
 
                for (int k = 0; k < (int)appointments.size(); k++)
                {
                    if (k == i)
                    {
                        continue;
                    }
 
                    if (appointments[k].doctorID ==
                            appointments[i].doctorID &&
                        appointments[k].appointmentDate ==
                            newDate &&
                        appointments[k].timeSlot ==
                            timeSlots[candidateIndex] &&
                        appointments[k].status != "Cancelled")
                    {
                        booked = true;
                        break;
                    }
                }
 
                if (booked)
                {
                    cout << "\n" << timeSlots[candidateIndex]
                         << " has already been booked.\n";
                    cout << "Please choose a different slot from the list above.\n";
                    continue;
                }
 
                newSlotIndex = candidateIndex;
                break;
            }
 
            string newTime =
                timeSlots[newSlotIndex];
 
            // Update appointment
            appointments[i].appointmentDate = newDate;
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
 
    // A queue represents patients physically waiting RIGHT NOW,
    // so it only ever makes sense to view TODAY's queue - no
    // date is asked for.
    string date = getTodayDateString();
 
    int doctorIndex = selectDoctor();
 
    if (doctorIndex == -1)
    {
        cout << "\nCancelled.\n";
        return;
    }
 
    int doctorID = doctorIDs[doctorIndex];
 
    cout << "\n";
    cout << "Queue for "
         << doctorNames[doctorIndex]
         << " on " << date << " (Today)" << endl;
 
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
                appointments[i].appointmentDate == date &&
                appointments[i].timeSlot == timeSlots[slot] &&
                appointments[i].status == "Pending")
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
 
    // Calling/serving a patient is a real-time action - it
    // only ever applies to TODAY's queue, never a future date.
    string date = getTodayDateString();
 
    int doctorIndex = selectDoctor();
 
    if (doctorIndex == -1)
    {
        cout << "\nCancelled.\n";
        return;
    }
 
    int doctorID = doctorIDs[doctorIndex];
 
    // --------------------------------------------------------
    // Show the FULL remaining queue first, so the user can see
    // who is waiting and in what order before anyone is called.
    // --------------------------------------------------------
 
    cout << "\nCurrent Queue for "
         << doctorNames[doctorIndex]
         << " on " << date << " (Today)\n";
 
    cout << "-------------------------------------------------------\n";
 
    cout << left
         << setw(10) << "Queue"
         << setw(15) << "Patient ID"
         << setw(15) << "Time"
         << setw(15) << "Appointment"
         << endl;
 
    cout << "-------------------------------------------------------\n";
 
    int queueNumber = 1;
    int nextIndex = -1; // vector index of the earliest waiting patient
 
    for (int slot = 0; slot < MAX_SLOTS; slot++)
    {
        for (int i = 0;
             i < (int)appointments.size();
             i++)
        {
            if (appointments[i].doctorID == doctorID &&
                appointments[i].appointmentDate == date &&
                appointments[i].timeSlot == timeSlots[slot] &&
                appointments[i].status == "Pending")
            {
                cout << left
                     << setw(10) << queueNumber
                     << setw(15) << appointments[i].patientID
                     << setw(15) << appointments[i].timeSlot
                     << setw(15) << appointments[i].appointmentID
                     << endl;
 
                // The first one found (chronologically) is next in line
                if (nextIndex == -1)
                {
                    nextIndex = i;
                }
 
                queueNumber++;
            }
        }
    }
 
    if (nextIndex == -1)
    {
        cout << "\nNo patients are waiting for this doctor today.\n";
        return;
    }
 
    // --------------------------------------------------------
    // Confirm before calling/serving the earliest patient
    // --------------------------------------------------------
 
    cout << "\nNext in line: Patient ID "
         << appointments[nextIndex].patientID
         << " at " << appointments[nextIndex].timeSlot << endl;
 
    cout << "1. Call this patient\n";
    cout << "2. Cancel\n";
 
    int confirm = getValidatedInt("Enter choice: ", 1, 2);
 
    if (confirm != 1)
    {
        cout << "\nCall cancelled.\n";
        return;
    }
 
    appointments[nextIndex].status = "Completed";
 
    cout << "\n";
    cout << "=========================================\n";
    cout << "             NEXT PATIENT\n";
    cout << "=========================================\n";
 
    cout << "Patient ID     : "
         << appointments[nextIndex].patientID << endl;
 
    cout << "Appointment ID : "
         << appointments[nextIndex].appointmentID << endl;
 
    cout << "Time           : "
         << appointments[nextIndex].timeSlot << endl;
 
    cout << "Status         : Now being served\n";
 
    cout << "=========================================\n";
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
 
    string date = getValidatedAppointmentDate("Enter Date (DD/MM/YYYY): ");
 
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
        if (appointments[i].appointmentDate == date)
        {
            found = true;
 
            int doctorIndex =
                getDoctorIndex(
                    appointments[i].doctorID);
 
            string doctorName =
                (doctorIndex != -1) ? doctorNames[doctorIndex] : "Unknown";
 
            cout << left
                 << setw(15)
                 << appointments[i].appointmentID
 
                 << setw(12)
                 << appointments[i].patientID
 
                 << setw(15)
                 << doctorName
 
                 << setw(15)
                 << appointments[i].timeSlot;
 
            // status is now the single source of truth:
            // "Pending" / "Completed" / "Cancelled"
            cout << setw(12) << appointments[i].status;
 
            cout << endl;
        }
    }
 
    if (!found)
    {
        cout << "No appointments found for this date.\n";
    }
}