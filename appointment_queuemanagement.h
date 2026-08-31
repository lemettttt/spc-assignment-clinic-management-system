#ifndef APPOINTMENT_QUEUEMANAGEMENT_H
#define APPOINTMENT_QUEUEMANAGEMENT_H

#include "clinic_shared.h"
#include <vector>

// Menu for appointment and queue management
void appointmentQueueMenu(vector<Appointment>& appointments,
                          vector<Patient>& patients);

#endif