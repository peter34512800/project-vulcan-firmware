/**********************************************************************
* File: SensorManager.h
* Author: Peter
* Description: 
*   Define Sensor functions and logging
* 
**********************************************************************/

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "PinConstants.h"
#include "HX711.h"

struct LogDataRow {
    uint32_t timestamp;
    int servoAngle;
    float scaleReading;
    float thrustReading;
    float pt1Reading;
    float pt2Reading;
};

void syncSavedData();
void streamDataRow(const LogDataRow& row);
void streamSerialPrint(const char* message);
void logDataToSD(const LogDataRow& row);
String getNextAvailableFilename();
float readHX711(HX711& adc);
float readScaleHX711();
float readThrustLoadCellHX711();
float calculatePTOffset(const int value);

#endif