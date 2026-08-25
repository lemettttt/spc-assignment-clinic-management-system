#ifndef LOGIN_MODULE_H
#define LOGIN_MODULE_H

#include "clinic_shared.h"

bool validatePasswordLength(const string& password);
bool checkDuplicateUsername(const string& username);
int  findStaffIndex(const string& username);

bool login(const vector<Staff>& staffList, const string& username, const string& password);

void registerUser();
bool loginUser();

// main.cpp会调用这个入口：一直显示Login/Register菜单，
// 直到登入成功，或失败达到MAX_LOGIN_ATTEMPTS次数后强制关闭系统
void authenticationGate();

#endif
