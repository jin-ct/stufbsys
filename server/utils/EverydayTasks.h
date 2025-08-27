#pragma once

#include <drogon/drogon.h>
#include <iostream>
#include <chrono>
#include "../models/DataStatistics.h"
#include "../models/TeacherDataStatistics.h"
#include "../models/Teacher.h"
#include "../models/PlatformSettings.h"
#include "../utils/utils.h"

void doList();
void startEverydayTasks();
void scheduleNextTask();
std::string getWeekDay();
void updateStatisticalData();
void updateTeacherStatisticalData();
void updateTeacherBottomSaying();