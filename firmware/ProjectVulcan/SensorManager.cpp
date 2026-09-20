/**********************************************************************
* File: SensorManager.cpp
* Author: Peter
* Description: 
*   Implement Sensor reading functions and logging
* 
**********************************************************************/

#include <Arduino.h>
#include <SD.h>
#include <QNEthernet.h>
#include <ADS1256.h>
#include <SPI.h>


#include "StateMachine.h"
#include "SensorManager.h"
#include "HX711.h"
#include "PinConstants.h"

namespace qn = qindesign::network;
extern qn::EthernetClient client;
extern const char* serverIP;
extern const int serverPort;
extern SystemStatus systemStatus;

extern HX711 scaleHX711;
extern HX711 thrustHX711;
extern File logFile;

// ADS1256 adc(ActivePins::ADS_DRDY_PIN, ActivePins::ADS_RESET_PIN, ActivePins::ADS_SYNC_PIN, ActivePins::ADS_CS_PIN, 2.500);

void syncSavedData() {
    // SD init check
    if (!systemStatus.sdInitialized) {
        Serial.println("Sync Failed: SD card not initialized.");
        systemStatus.logDataSync = false;
        return;
    }

    // CSV file check, if not exist there are no data available
    if (!SD.exists("data.csv")) {
        Serial.println("Sync Skipped: data.csv not found.");
        systemStatus.logDataSync = true;
        return;
    }

    Serial.println("Attempting to connect for startup data sync...");
    if (client.connect(serverIP, serverPort)) {
        Serial.println("SUCCESS: Connected to server, performing data sync.");

        client.println("MODE:DATA_SYNC");
        client.flush();

        File logFile = SD.open("data.csv", FILE_READ);
        if (logFile) {
            Serial.println("Syncing data.csv to server...");
            uint8_t buffer[256];
            
            while (logFile.available()) {
                int bytesRead = logFile.read(buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    client.write(buffer, bytesRead);
                }
            }

            client.flush();
            logFile.close();
        }
        client.stop();
        systemStatus.logDataSync = true;
    }
    else {
        Serial.println("FAILED: Teensy not connected to server.");
        systemStatus.logDataSync = false;
    }

}

void streamDataRow(const LogDataRow& row) {
    if (client.connected()) {
        client.println("MODE:DATA_LOG");
        String data = 
            String(row.timestamp) + "," +
            String(row.servoAngle) + "," +
            String(row.scaleReading, 4) + "," +
            String(row.thrustReading, 4) + "," +
            String(row.pt1Reading) + "," +
            String(row.pt2Reading) + "\n";
        client.print(data);
        // make sure last data column use println
        client.flush();
    }
}

void streamSerialPrint(const char* message) {
    if (client.connected()) {
        client.println("MODE:SERIAL_PRINT");
        client.print("PRINT:");
        client.println(message);
        client.flush();
    }
}

void logDataToSD(const LogDataRow& row) {
    if (!systemStatus.sdInitialized || !logFile) return;
    

    static String prevLine = "";
    static int rowCount = 0;
    
    // unsigned long t = micros();
    String line = String(row.timestamp) + "," +
                  String(row.servoAngle) + "," +
                  String(row.scaleReading, 4) + "\n";
    
    if (prevLine != line) {
        logFile.print(line);   // one write call instead of five
        prevLine = line;
        rowCount++;
    }

    if (rowCount >= 0) {
        unsigned long t0 = micros();
        logFile.flush();
        rowCount = 0;

        unsigned long dt = micros() - t0;
        if (dt > 20000) {   // only log calls slower than 20ms
            Serial.print("SLOW FLUSH logDataToSD: ");
            Serial.print(dt);   
            Serial.println(" us");
        }
    }
}

String getNextAvailableFilename() {
    String filename = "data.csv";
    if (!SD.exists(filename.c_str())) {
        return filename;
    }

    int index = 1;
    while (true) {
        filename = "data" + String(index) + ".csv";
        if (!SD.exists(filename.c_str())) {
            return filename;
        }
        index++;
    }
}

float readHX711(HX711& adc) {
    //TODO change the lastreading, it 
    static float lastReading = 0;
    if (adc.is_ready()) {
        // calibrated reading
        // return adc.get_units();
        lastReading = adc.get_units();
        // lastReading = adc.read();
        // Serial.print("Scale Reading: ");
        // Serial.println(lastReading);
    }
    return lastReading;
    // return 0;
}

float readScaleHX711() {
    static float lastReading = 0;
    if (scaleHX711.is_ready()) {
        lastReading = scaleHX711.get_units();
    }
    return lastReading;
}
float readThrustLoadCellHX711() {
    static float lastReading = 0;
    if (thrustHX711.is_ready()) {
        lastReading = thrustHX711.get_units();
    }
    return lastReading;
}

float calculatePTOffset(const int value) {
    return (value - 55) * 1.4385;
}
