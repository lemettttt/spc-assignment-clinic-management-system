/*
    ============================================================
    Appointment & Queue Management Module
    Pah Yan Han
    ============================================================
 */
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
// MAX_DOCTORS, doctorIDs[] are defined in clinic_shared.h.
// Do not redeclare them here.

const int MAX_SLOTS = 12;
const int MAX_YEARS_AHEAD = 2;          // Max future years for booking

// ============================================================
// FIXED TIME SLOTS
// ============================================================

string timeSlots[MAX_SLOTS] =
{
    "09:00 AM", "09:30 AM", "10:00 AM", "10:30 AM",
    "11:00 AM", "11:30 AM",
    "02:00 PM", "02:30 PM", "03:00 PM", "03:30 PM",
    "04:00 PM", "04:30 PM"
};

// ============================================================
// DOCTOR AVAILABILITY (true=available, false=unavailable)
// Row 0: Dr. Ahmad, Row 1: Dr. Lim, Row 2: Dr. Kumar
// ============================================================

bool doctorAvailability[MAX_DOCTORS][MAX_SLOTS] =
{
    // Dr. Ahmad
    { true, true, true, true, true, true,
      false, false, true, true, true, false },
    // Dr. Lim
    { false, false, true, true, true, false,
      true, true, true, true, false, false },
    // Dr. Kumar
    { true, true, true, false, false, false,
      true, true, true, false, false, false }
};

// ============================================================
// FUNCTION PROTOTYPES
// ============================================================

bool patientExists(const vector<Patient>& patients, int patientID);
bool isValidDate(string date);
bool isDoctorAvailable(int doctorIndex, int slotIndex);
bool checkSlotAvailability(const vector<Appointment>& appointments, int doctorID, string date, string timeSlot);
int getDoctorIndex(int doctorID);
string getTodayDateString();
bool isSlotInPast(const string& date, const string& timeSlot);
string getValidatedAppointmentDate(const string& prompt);
bool isDoctorClinicClosedForToday(int doctorIndex);
int selectDoctor();
int generateNextAppointmentID(const vector<Appointment>& appointments);
void bookAppointment(vector<Appointment>& appointments, vector<Patient>& patients);
void displayDoctorSchedule(const vector<Appointment>& appointments);
void cancelAppointment(vector<Appointment>& appointments);
void rescheduleAppointment(vector<Appointment>& appointments);
void manageQueue(vector<Appointment>& appointments, vector<Patient>& patients);
void getNextPatientInQueue(vector<Appointment>& appointments);
void displayTodayAppointments(const vector<Appointment>& appointments);

// ============================================================
// PATIENT EXISTENCE CHECK
// ============================================================

bool patientExists(const vector<Patient>& patients, int patientID) {
    for (int i = 0; i < (int)patients.size(); i++) {
        if (patients[i].patientID == patientID) return true;
    }
    return false;
}

// ============================================================
// GET DOCTOR ARRAY INDEX
// ============================================================

int getDoctorIndex(int doctorID) {
    for (int i = 0; i < MAX_DOCTORS; i++) {
        if (doctorIDs[i] == doctorID) return i;
    }
    return -1;
}

// ============================================================
// GET TODAY'S DATE (DD/MM/YYYY)
// ============================================================

string getTodayDateString() {
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;
    ostringstream oss;
    oss << setfill('0') << setw(2) << day << "/"
        << setfill('0') << setw(2) << month << "/"
        << year;
    return oss.str();
}

// ============================================================
// CHECK IF A SLOT IS ALREADY IN THE PAST (only for today)
// ============================================================

bool isSlotInPast(const string& date, const string& timeSlot) {
    if (date != getTodayDateString()) return false;
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int hour = stoi(timeSlot.substr(0, 2));
    int minute = stoi(timeSlot.substr(3, 2));
    string period = timeSlot.substr(6, 2);
    if (period == "PM" && hour != 12) hour += 12;
    if (period == "AM" && hour == 12) hour = 0;
    int slotMinutes = hour * 60 + minute;
    int nowMinutes = localTime->tm_hour * 60 + localTime->tm_min;
    return slotMinutes <= nowMinutes;
}

// ============================================================
// GENERIC VALIDATED INTEGER INPUT
// ============================================================

// ============================================================
// NOTE: no local integer-input function here anymore — this file
// now uses the shared getValidatedMenuChoice() from input_utils.h for
// all range-checked integer input. The old local version used raw
// cin >> which left a leftover '\n' in the buffer after every call,
// causing the next getline()-based prompt to silently read an empty
// line and skip itself.
// ============================================================

// ============================================================
// CHECK IF DOCTOR CLINIC HOURS HAVE ENDED FOR TODAY
// ============================================================

bool isDoctorClinicClosedForToday(int doctorIndex) {
    time_t rawTime = time(nullptr);
    tm* localTime = localtime(&rawTime);
    int nowMinutes = localTime->tm_hour * 60 + localTime->tm_min;
    int latestSlotMinutes = -1;
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (!isDoctorAvailable(doctorIndex, i)) continue;
        string timeSlot = timeSlots[i];
        int hour = stoi(timeSlot.substr(0, 2));
        int minute = stoi(timeSlot.substr(3, 2));
        string period = timeSlot.substr(6, 2);
        if (period == "PM" && hour != 12) hour += 12;
        if (period == "AM" && hour == 12) hour = 0;
        int slotMinutes = hour * 60 + minute;
        if (slotMinutes > latestSlotMinutes) latestSlotMinutes = slotMinutes;
    }
    return (latestSlotMinutes != -1 && nowMinutes >= latestSlotMinutes);
}

// ============================================================
// VALIDATED APPOINTMENT DATE (with 2‑year future limit)
// ============================================================

string getValidatedAppointmentDate(const string& prompt) {
    string date;
    while (true) {
        date = getValidatedFutureDate(prompt);   // from input_utils.h
        time_t rawTime = time(nullptr);
        tm* localTime = localtime(&rawTime);
        int todayYear = localTime->tm_year + 1900;
        int inputYear = stoi(date.substr(6, 4));
        if (inputYear <= todayYear + MAX_YEARS_AHEAD) return date;
        cout << "Error: Date cannot be more than " << MAX_YEARS_AHEAD << " years in the future.\n";
    }
}

// ============================================================
// SELECT DOCTOR (returns 0‑based index or -1 if cancelled)
// ============================================================

int selectDoctor() {
    cout << "\nAvailable Doctors:\n";
    cout << "0. Cancel / Back\n";
    for (int i = 0; i < MAX_DOCTORS; i++) {
        cout << i + 1 << ". " << doctorNames[i] << " (ID: " << doctorIDs[i] << ")\n";
    }
    int choice = getValidatedMenuChoice("Select Doctor (0 to cancel, 1-" + to_string(MAX_DOCTORS) + "): ",
                                  0, MAX_DOCTORS);
    return (choice == 0) ? -1 : choice - 1;
}

// ============================================================
// CHECK DOCTOR'S FIXED WORKING SCHEDULE
// ============================================================

bool isDoctorAvailable(int doctorIndex, int slotIndex) {
    if (doctorIndex < 0 || doctorIndex >= MAX_DOCTORS) return false;
    if (slotIndex < 0 || slotIndex >= MAX_SLOTS) return false;
    return doctorAvailability[doctorIndex][slotIndex];
}

// ============================================================
// CHECK IF A SLOT IS ALREADY BOOKED
// ============================================================

bool checkSlotAvailability(const vector<Appointment>& appointments, int doctorID,
                           string date, string timeSlot) {
    for (int i = 0; i < (int)appointments.size(); i++) {
        if (appointments[i].doctorID == doctorID &&
            appointments[i].appointmentDate == date &&
            appointments[i].timeSlot == timeSlot &&
            appointments[i].status != "Cancelled") {
            return false;
        }
    }
    return true;
}

// ============================================================
// GENERATE NEXT APPOINTMENT ID
// ============================================================

int generateNextAppointmentID(const vector<Appointment>& appointments) {
    int maxID = 1000;
    for (int i = 0; i < (int)appointments.size(); i++) {
        if (appointments[i].appointmentID > maxID) maxID = appointments[i].appointmentID;
    }
    return maxID + 1;
}

// ============================================================
// MAIN MENU
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

        choice = getValidatedMenuChoice("  Enter your choice: ", 0, 7);

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

void bookAppointment(vector<Appointment>& appointments, vector<Patient>& patients) {
    Appointment newAppointment;

    cout << "\n=========================================\n";
    cout << "           BOOK APPOINTMENT\n";
    cout << "=========================================\n";

    // Patient ID
    newAppointment.patientID = getValidatedMenuChoice("Enter Patient ID (0 to cancel): ", 0, 999999999);
    if (newAppointment.patientID == 0) {
        cout << "\nBooking cancelled.\n";
        return;
    }
    if (!patientExists(patients, newAppointment.patientID)) {
        cout << "\nPatient ID not found.\n";
        cout << "Please register the patient first.\n";
        return;
    }

    // Date
    newAppointment.appointmentDate = getValidatedAppointmentDate("\nEnter Appointment Date (DD/MM/YYYY): ");

    // Doctor
    int doctorIndex = selectDoctor();
    if (doctorIndex == -1) {
        cout << "\nBooking cancelled.\n";
        return;
    }
    newAppointment.doctorID = doctorIDs[doctorIndex];

    // If today and clinic hours ended, force re‑entry of date
    if (newAppointment.appointmentDate == getTodayDateString() &&
        isDoctorClinicClosedForToday(doctorIndex)) {
        cout << "\n[System] " << doctorNames[doctorIndex]
             << "'s clinic hours have ended for today.\n";
        cout << "[System] Please select tomorrow or a later date.\n";
        newAppointment.appointmentDate = getValidatedAppointmentDate(
            "\nPlease enter again the appointment date (DD/MM/YYYY): ");
        while (newAppointment.appointmentDate == getTodayDateString() &&
               isDoctorClinicClosedForToday(doctorIndex)) {
            cout << "\n[System] You entered today's date, but the doctor's clinic hours have already ended.\n";
            newAppointment.appointmentDate = getValidatedAppointmentDate(
                "\nPlease enter again the appointment date (DD/MM/YYYY): ");
        }
    }

    // Show working slots (numbered 1..N)
    cout << "\nAvailable Time Slots for " << doctorNames[doctorIndex] << ":\n\n";
    cout << left << setw(5) << "No." << setw(12) << "Time" << setw(12) << "Status" << endl;
    cout << "-----------------------------\n";

    int slotIndexMap[MAX_SLOTS];
    int slotCount = 0;
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (!isDoctorAvailable(doctorIndex, i)) continue;
        slotIndexMap[slotCount] = i;
        slotCount++;
        cout << left << setw(5) << slotCount << setw(12) << timeSlots[i];
        if (isSlotInPast(newAppointment.appointmentDate, timeSlots[i]))
            cout << "Passed";
        else if (!checkSlotAvailability(appointments, newAppointment.doctorID,
                                       newAppointment.appointmentDate, timeSlots[i]))
            cout << "Booked";
        else
            cout << "Available";
        cout << endl;
    }
    if (slotCount == 0) {
        cout << "\nThis doctor has no working slots.\n";
        return;
    }

    // Select time slot (loop until a free one is chosen or cancelled)
    int slotIndex = -1;
    while (true) {
        int slotChoice = getValidatedMenuChoice("\nSelect Time Slot (0 to cancel, 1-" +
                                         to_string(slotCount) + "): ", 0, slotCount);
        if (slotChoice == 0) {
            cout << "\nBooking cancelled.\n";
            return;
        }
        int candidateIndex = slotIndexMap[slotChoice - 1];
        if (isSlotInPast(newAppointment.appointmentDate, timeSlots[candidateIndex])) {
            cout << "\n" << timeSlots[candidateIndex] << " has already passed for today.\n";
            cout << "Please choose a different slot from the list above.\n";
            continue;
        }
        if (!checkSlotAvailability(appointments, newAppointment.doctorID,
                                   newAppointment.appointmentDate, timeSlots[candidateIndex])) {
            cout << "\n" << timeSlots[candidateIndex] << " has already been booked.\n";
            cout << "Please choose a different slot from the list above.\n";
            continue;
        }
        slotIndex = candidateIndex;
        break;
    }
    newAppointment.timeSlot = timeSlots[slotIndex];

    // Generate ID
    newAppointment.appointmentID = generateNextAppointmentID(appointments);

    // Set doctor type from shared doctor list
    newAppointment.doctorType = "General";
    for (int i = 0; i < (int)doctors.size(); i++) {
        if (doctors[i].doctorID == newAppointment.doctorID) {
            newAppointment.doctorType = doctors[i].specialization;
            break;
        }
    }
    newAppointment.status = "Pending";
    appointments.push_back(newAppointment);

    // Confirmation
    cout << "\n=========================================\n";
    cout << "     APPOINTMENT BOOKED SUCCESSFULLY\n";
    cout << "=========================================\n";
    cout << left << setw(20) << "Appointment ID:" << newAppointment.appointmentID << endl;
    cout << left << setw(20) << "Patient ID:" << newAppointment.patientID << endl;
    cout << left << setw(20) << "Doctor:" << doctorNames[doctorIndex] << endl;
    cout << left << setw(20) << "Date:" << newAppointment.appointmentDate << endl;
    cout << left << setw(20) << "Time:" << newAppointment.timeSlot << endl;
    cout << left << setw(20) << "Status:" << newAppointment.status << endl;
    cout << "=========================================\n";
}

// ============================================================
// DISPLAY DOCTOR SCHEDULE
// ============================================================

void displayDoctorSchedule(const vector<Appointment>& appointments) {
    cout << "\n=========================================\n";
    cout << "        DISPLAY DOCTOR SCHEDULE\n";
    cout << "=========================================\n";

    int doctorIndex = selectDoctor();
    if (doctorIndex == -1) {
        cout << "\nCancelled.\n";
        return;
    }

    string date = getValidatedAppointmentDate("Enter Date (DD/MM/YYYY): ");
    if (date == getTodayDateString() && isDoctorClinicClosedForToday(doctorIndex)) {
        cout << "\n[System] " << doctorNames[doctorIndex]
             << "'s clinic hours have ended for today. Cannot display today's schedule.\n";
        return;
    }

    cout << "\nDoctor : " << doctorNames[doctorIndex] << endl;
    cout << "Date   : " << date << endl;
    cout << "\n" << left << setw(5) << "No." << setw(12) << "Time" << setw(12) << "Status" << endl;
    cout << "-----------------------------\n";

    for (int i = 0; i < MAX_SLOTS; i++) {
        cout << left << setw(5) << i + 1 << setw(12) << timeSlots[i];
        if (!isDoctorAvailable(doctorIndex, i))
            cout << "Unavailable";
        else if (isSlotInPast(date, timeSlots[i]))
            cout << "Passed";
        else {
            bool booked = !checkSlotAvailability(appointments, doctorIDs[doctorIndex], date, timeSlots[i]);
            cout << (booked ? "Booked" : "Available");
        }
        cout << endl;
    }
}

// ============================================================
// CANCEL APPOINTMENT
// ============================================================

void cancelAppointment(vector<Appointment>& appointments) {
    cout << "\n=========================================\n";
    cout << "          CANCEL APPOINTMENT\n";
    cout << "=========================================\n";

    int appointmentID = getValidatedMenuChoice("Enter Appointment ID (0 to cancel): ", 0, 999999999);
    if (appointmentID == 0) {
        cout << "\nCancelled.\n";
        return;
    }

    for (int i = 0; i < (int)appointments.size(); i++) {
        if (appointments[i].appointmentID == appointmentID) {
            if (appointments[i].status == "Cancelled") {
                cout << "This appointment is already cancelled.\n";
                return;
            }
            if (appointments[i].status == "Completed") {
                cout << "This appointment has already been served and cannot be cancelled.\n";
                return;
            }
            cout << "\nAppointment Found\n";
            cout << "Patient ID : " << appointments[i].patientID << endl;
            cout << "Doctor ID  : " << appointments[i].doctorID << endl;
            cout << "Date       : " << appointments[i].appointmentDate << endl;
            cout << "Time       : " << appointments[i].timeSlot << endl;
            cout << "\nAre you sure you want to cancel?\n";
            cout << "1. Yes\n2. No\n";
            int confirm = getValidatedMenuChoice("Enter choice: ", 1, 2);
            if (confirm == 1) {
                appointments[i].status = "Cancelled";
                cout << "\nAppointment cancelled successfully.\n";
            } else {
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

void rescheduleAppointment(vector<Appointment>& appointments) {
    cout << "\n=========================================\n";
    cout << "         RESCHEDULE APPOINTMENT\n";
    cout << "=========================================\n";

    int appointmentID = getValidatedMenuChoice("Enter Appointment ID (0 to cancel): ", 0, 999999999);
    if (appointmentID == 0) {
        cout << "\nCancelled.\n";
        return;
    }

    for (int i = 0; i < (int)appointments.size(); i++) {
        if (appointments[i].appointmentID == appointmentID) {
            if (appointments[i].status == "Cancelled") {
                cout << "This appointment has already been cancelled.\n";
                return;
            }
            if (appointments[i].status == "Completed") {
                cout << "This appointment has already been served and cannot be rescheduled.\n";
                return;
            }

            int doctorIndex = getDoctorIndex(appointments[i].doctorID);
            if (doctorIndex == -1) {
                cout << "\nError: Doctor record not found for this appointment.\n";
                return;
            }

            cout << "\nCurrent Appointment\n";
            cout << "Patient ID : " << appointments[i].patientID << endl;
            cout << "Doctor     : " << doctorNames[doctorIndex] << endl;
            cout << "Date       : " << appointments[i].appointmentDate << endl;
            cout << "Time       : " << appointments[i].timeSlot << endl;

            // New date
            string newDate = getValidatedAppointmentDate("\nEnter New Date (DD/MM/YYYY): ");

            // Show working slots
            cout << "\nAvailable Time Slots:\n\n";
            cout << left << setw(5) << "No." << setw(12) << "Time" << setw(12) << "Status" << endl;
            cout << "-----------------------------\n";

            int slotIndexMap[MAX_SLOTS];
            int slotCount = 0;
            for (int j = 0; j < MAX_SLOTS; j++) {
                if (!isDoctorAvailable(doctorIndex, j)) continue;
                slotIndexMap[slotCount] = j;
                slotCount++;
                cout << left << setw(5) << slotCount << setw(12) << timeSlots[j];
                if (isSlotInPast(newDate, timeSlots[j]))
                    cout << "Passed";
                else {
                    bool booked = false;
                    for (int k = 0; k < (int)appointments.size(); k++) {
                        if (k == i) continue;
                        if (appointments[k].doctorID == appointments[i].doctorID &&
                            appointments[k].appointmentDate == newDate &&
                            appointments[k].timeSlot == timeSlots[j] &&
                            appointments[k].status != "Cancelled") {
                            booked = true;
                            break;
                        }
                    }
                    cout << (booked ? "Booked" : "Available");
                }
                cout << endl;
            }
            if (slotCount == 0) {
                cout << "\nThis doctor has no working slots.\n";
                return;
            }

            // Select new slot
            int newSlotIndex = -1;
            while (true) {
                int newSlot = getValidatedMenuChoice("\nSelect New Time Slot (0 to cancel, 1-" +
                                              to_string(slotCount) + "): ", 0, slotCount);
                if (newSlot == 0) {
                    cout << "\nReschedule cancelled.\n";
                    return;
                }
                int candidateIndex = slotIndexMap[newSlot - 1];
                if (isSlotInPast(newDate, timeSlots[candidateIndex])) {
                    cout << "\n" << timeSlots[candidateIndex] << " has already passed for today.\n";
                    cout << "Please choose a different slot from the list above.\n";
                    continue;
                }
                bool booked = false;
                for (int k = 0; k < (int)appointments.size(); k++) {
                    if (k == i) continue;
                    if (appointments[k].doctorID == appointments[i].doctorID &&
                        appointments[k].appointmentDate == newDate &&
                        appointments[k].timeSlot == timeSlots[candidateIndex] &&
                        appointments[k].status != "Cancelled") {
                        booked = true;
                        break;
                    }
                }
                if (booked) {
                    cout << "\n" << timeSlots[candidateIndex] << " has already been booked.\n";
                    cout << "Please choose a different slot from the list above.\n";
                    continue;
                }
                newSlotIndex = candidateIndex;
                break;
            }

            string newTime = timeSlots[newSlotIndex];
            appointments[i].appointmentDate = newDate;
            appointments[i].timeSlot = newTime;

            cout << "\n=========================================\n";
            cout << "     APPOINTMENT RESCHEDULED\n";
            cout << "=========================================\n";
            cout << "Appointment ID : " << appointments[i].appointmentID << endl;
            cout << "New Date       : " << newDate << endl;
            cout << "New Time       : " << newTime << endl;
            cout << "=========================================\n";
            return;
        }
    }
    cout << "Appointment ID not found.\n";
}

// ============================================================
// MANAGE QUEUE (TODAY ONLY)
// ============================================================

void manageQueue(vector<Appointment>& appointments, vector<Patient>& patients) {
    cout << "\n=========================================\n";
    cout << "          PATIENT QUEUE\n";
    cout << "=========================================\n";

    string date = getTodayDateString();
    int doctorIndex = selectDoctor();
    if (doctorIndex == -1) {
        cout << "\nCancelled.\n";
        return;
    }
    int doctorID = doctorIDs[doctorIndex];

    cout << "\nQueue for " << doctorNames[doctorIndex] << " on " << date << " (Today)\n";
    cout << "-------------------------------------------------------\n";
    cout << left << setw(10) << "Queue" << setw(15) << "Patient ID"
         << setw(15) << "Time" << setw(15) << "Appointment" << endl;
    cout << "-------------------------------------------------------\n";

    int queueNumber = 1;
    for (int slot = 0; slot < MAX_SLOTS; slot++) {
        for (int i = 0; i < (int)appointments.size(); i++) {
            if (appointments[i].doctorID == doctorID &&
                appointments[i].appointmentDate == date &&
                appointments[i].timeSlot == timeSlots[slot] &&
                appointments[i].status == "Pending") {
                cout << left << setw(10) << queueNumber
                     << setw(15) << appointments[i].patientID
                     << setw(15) << appointments[i].timeSlot
                     << setw(15) << appointments[i].appointmentID << endl;
                queueNumber++;
            }
        }
    }
    if (queueNumber == 1) cout << "No patients are currently waiting.\n";
}

// ============================================================
// CALL NEXT PATIENT
// ============================================================

void getNextPatientInQueue(vector<Appointment>& appointments) {
    cout << "\n=========================================\n";
    cout << "           CALL NEXT PATIENT\n";
    cout << "=========================================\n";

    string date = getTodayDateString();
    int doctorIndex = selectDoctor();
    if (doctorIndex == -1) {
        cout << "\nCancelled.\n";
        return;
    }
    int doctorID = doctorIDs[doctorIndex];

    // Show remaining queue
    cout << "\nCurrent Queue for " << doctorNames[doctorIndex] << " on " << date << " (Today)\n";
    cout << "-------------------------------------------------------\n";
    cout << left << setw(10) << "Queue" << setw(15) << "Patient ID"
         << setw(15) << "Time" << setw(15) << "Appointment" << endl;
    cout << "-------------------------------------------------------\n";

    int queueNumber = 1;
    int nextIndex = -1;
    for (int slot = 0; slot < MAX_SLOTS; slot++) {
        for (int i = 0; i < (int)appointments.size(); i++) {
            if (appointments[i].doctorID == doctorID &&
                appointments[i].appointmentDate == date &&
                appointments[i].timeSlot == timeSlots[slot] &&
                appointments[i].status == "Pending") {
                cout << left << setw(10) << queueNumber
                     << setw(15) << appointments[i].patientID
                     << setw(15) << appointments[i].timeSlot
                     << setw(15) << appointments[i].appointmentID << endl;
                if (nextIndex == -1) nextIndex = i;
                queueNumber++;
            }
        }
    }
    if (nextIndex == -1) {
        cout << "\nNo patients are waiting for this doctor today.\n";
        return;
    }

    // Confirm and serve
    cout << "\nNext in line: Patient ID " << appointments[nextIndex].patientID
         << " at " << appointments[nextIndex].timeSlot << endl;
    cout << "1. Call this patient\n2. Cancel\n";
    int confirm = getValidatedMenuChoice("Enter choice: ", 1, 2);
    if (confirm != 1) {
        cout << "\nCall cancelled.\n";
        return;
    }
    appointments[nextIndex].status = "Completed";

    cout << "\n=========================================\n";
    cout << "             NEXT PATIENT\n";
    cout << "=========================================\n";
    cout << "Patient ID     : " << appointments[nextIndex].patientID << endl;
    cout << "Appointment ID : " << appointments[nextIndex].appointmentID << endl;
    cout << "Time           : " << appointments[nextIndex].timeSlot << endl;
    cout << "Status         : Now being served\n";
    cout << "=========================================\n";
}

// ============================================================
// DISPLAY APPOINTMENTS BY DATE
// ============================================================

void displayTodayAppointments(const vector<Appointment>& appointments) {
    cout << "\n=========================================\n";
    cout << "       TODAY'S APPOINTMENTS\n";
    cout << "=========================================\n";

    string date = getValidatedAppointmentDate("Enter Date (DD/MM/YYYY): ");
    cout << "\nAppointments on " << date << endl;
    cout << "---------------------------------------------------------------------\n";
    cout << left << setw(15) << "Appointment" << setw(12) << "Patient"
         << setw(15) << "Doctor" << setw(15) << "Time" << setw(12) << "Status" << endl;
    cout << "---------------------------------------------------------------------\n";

    bool found = false;
    for (int i = 0; i < (int)appointments.size(); i++) {
        if (appointments[i].appointmentDate == date) {
            found = true;
            int doctorIndex = getDoctorIndex(appointments[i].doctorID);
            string doctorName = (doctorIndex != -1) ? doctorNames[doctorIndex] : "Unknown";
            cout << left << setw(15) << appointments[i].appointmentID
                 << setw(12) << appointments[i].patientID
                 << setw(15) << doctorName
                 << setw(15) << appointments[i].timeSlot
                 << setw(12) << appointments[i].status << endl;
        }
    }
    if (!found) cout << "No appointments found for this date.\n";
}