#include "PinConstants.h"
#include "StateMachine.h"
#include "ServoControl.h"
#include "SensorManager.h"
#include "HX711.h"

#include <SD.h>
#include <QNEthernet.h>
#include <Servo.h>
#include <format>
#include <string>

SystemStatus systemStatus;

const int chipSelect = BUILTIN_SDCARD;

namespace qn = qindesign::network;
qn::EthernetClient client;

const IPAddress staticIP(192, 168, 1, 100);    // Teensy's forced IP address
const IPAddress netmask(255, 255, 255, 0);     // Must match laptop
const IPAddress gateway(192, 168, 1, 1);       // Dummy gateway

// change this to laptop ethernet ip address
const char* serverIP = "192.168.1.10";
extern const int serverPort = 5002;

File logFile;

Servo oServo;
HX711 scaleHX711;
HX711 thrustHX711;

String logFilename;

elapsedMillis sampleTimer;
const unsigned long sampleInterval = 12;

elapsedMillis printTimer;
const unsigned long printInterval = 10;

// Hardware timer variables
unsigned long countdownStartTime = 0;
extern const unsigned long COUNTDOWN_LIMIT_MS = 180000;

extern int servoAngle;

// ---- HX711 Interrupt ----
volatile bool scaleHX711Ready = false;
volatile bool thrustHX711Ready = false;

void scaleHX711ISR() {
    scaleHX711Ready = true;
}

void thrustHX711ISR() {
    thrustHX711Ready = true;
}
// -------------------------

void setup() {
    systemStatus = SystemStatus();

    Serial.begin(115200);
    analogReadResolution(10);
    delay(5000);
    

    // SD Card initialization
    Serial.print("Initializing SD Card...");
    while (!SD.begin(chipSelect)) {
        systemStatus.sdInitialized = false;
        Serial.println("SD Card initialization: FAILED");
        delay(2000);
    }
    logFilename = getNextAvailableFilename();
    logFile = SD.open(logFilename.c_str(), FILE_WRITE);

    Serial.println("SD Card initialization: SUCCESS");
    systemStatus.sdInitialized = true;

    // Ethernet initialization
    qn::Ethernet.begin(staticIP, netmask, gateway);
    Serial.print("Obtaining Teensy ip address...\n");
    // while (qn::Ethernet.localIP() == INADDR_NONE) {
    //     delay(100);
    // }
    Serial.print("Teensy IP: ");
    Serial.println(qn::Ethernet.localIP());

    //INPUT_PULLUP pressed is LOW
    //INPUT_PULLDOWN pressed is HIGH
    pinMode(ActivePins::IGNITER_PIN, OUTPUT);
    pinMode(ActivePins::IGNITER_SWITCH_PIN, INPUT_PULLUP);
    pinMode(ActivePins::TRICKLE_SWITCH_PIN, INPUT_PULLUP);
    pinMode(ActivePins::FULL_FLOW_SWITCH_PIN, INPUT_PULLUP);
    pinMode(ActivePins::RESET_SWITCH_PIN, INPUT_PULLUP);
    pinMode(ActivePins::LED_PIN, OUTPUT);

    

    oServo.attach(ActivePins::SERVO_PIN);
    closeServo(oServo);
    

    // Serial.println("Servo attached");
    // oServo.write(180);
    // Serial.println("Servo moved to closed position");

    delay(1000);
    
    if (qn::Ethernet.linkState() == 1) {
        syncSavedData();
    } else {
        Serial.println("Ethernet link DOWN on boot. Retrying startup sync in loop.");
    }

    Serial.println("Begin HX711 calibration");
    
    //scale calibration and interrupt (grams)
    scaleHX711.begin(ActivePins::SCALE_DOUT_PIN, ActivePins::SCALE_SCK_PIN);
    scaleHX711.set_scale(11.675);
    scaleHX711.tare();
    attachInterrupt(digitalPinToInterrupt(ActivePins::SCALE_DOUT_PIN), scaleHX711ISR, FALLING);
    
    Serial.println("Finish HX711 scale calibration");

    //loadcell calibration and interrupt (grams)
    thrustHX711.begin(ActivePins::THRUST_DOUT_PIN, ActivePins::THRUST_SCK_PIN);
    thrustHX711.set_scale(221.65);
    // thrustHX711.tare();
    attachInterrupt(digitalPinToInterrupt(ActivePins::THRUST_DOUT_PIN), thrustHX711ISR, FALLING);

    Serial.println("Finish HX711 thrust calibration");

    Serial.println("Finish HX711 calibration");
    // initADS1256();

    // pinMode(23, OUTPUT);
    // digitalWrite(23, HIGH);

    Serial.println("timestamp_ms,servo_angle,scale_reading,thrust_reading,pt1,pt2");
    for (int i = 0; i < 3; ++i) {
        digitalWrite(ActivePins::LED_PIN, HIGH);
        delay(250);
        digitalWrite(ActivePins::LED_PIN, LOW);
        delay(250);
    }

}

void loop() {
    
    // State Machine operation
    updateStateMachine();
    
    // SD card presence veirification
    if (!SD.mediaPresent() || !systemStatus.sdInitialized) {
        if (systemStatus.sdInitialized) {
            Serial.println("WARNING: SD Card was physicially removed");
            if (logFile) {
                logFile.close();
                systemStatus.sdInitialized = false;
            }
        }

        if (SD.begin(chipSelect)) {
            logFile = SD.open(logFilename.c_str(), FILE_WRITE);
            if (logFile) {
                Serial.println("SD Card re-initialization: SUCCESS");
                systemStatus.sdInitialized = true;
            }
        }
    }

    // Serial.print("Pin 23: ");
    // Serial.println(digitalRead(23) == HIGH ? "HIGH" : "LOW");
    // Ethernet connectivity operation
    // Serial.print("Ethernet Link: ");
    // if (qn::Ethernet.linkState()) {
    //     Serial.println("UP");
    // } else {
    //     Serial.println("DOWN");
    // }
    
    // Check connectivity
    // static elapsedMillis printTimer;
    // if (printTimer >= 5000) {
    //     printTimer = 0;
    //     Serial.print("Ethernet Link: ");
    //     Serial.println(qn::Ethernet.linkState() ? "UP" : "DOWN");
    // }

    if (!client.connected()) {
        static elapsedMillis reconnectTimer;
        // Attempt server reconnection every 2 seconds
        if (reconnectTimer >= 2000) {
            client.stop();
            reconnectTimer = 0;
            Serial.print("Connecting to server... ");
            if (client.connect(serverIP, serverPort)) {
                Serial.println("SUCCESS");
            } else {
                Serial.println("FAILED");
            }
        }
    }

    // CSV data sync
    // if (!systemStatus.logDataSync) {
    //     static elapsedMillis syncRetryTimer;
    //     // Attempt data sync every 3 seconds
    //     if (syncRetryTimer >= 3000){ 
    //         syncRetryTimer = 0;
    //         Serial.println("Reattempting log data sync...");
    //         syncSavedData();
    //     }
    // }

    // Data stream and logging
    bool timerTriggered = (sampleTimer >= sampleInterval);

    if (timerTriggered || thrustHX711Ready || scaleHX711Ready) {
        sampleTimer = 0;
        noInterrupts();
        thrustHX711Ready = false;
        scaleHX711Ready = false;
        interrupts();

        LogDataRow currentFrame;
        currentFrame.timestamp = millis();
        
        currentFrame.servoAngle = servoAngle;

        float scaleReading = readScaleHX711();
        currentFrame.scaleReading = scaleReading;
        // currentFrame.scaleReading = 0;

        float thrustReading = readThrustLoadCellHX711();
        currentFrame.thrustReading = thrustReading;
        // currentFrame.thrustReading = 0;

        currentFrame.pt1Reading = calculatePTOffset(analogRead(ActivePins::PT_ONE_PIN));
        currentFrame.pt2Reading = calculatePTOffset(analogRead(ActivePins::PT_TWO_PIN));
        // Always log data to SD
        // logDataToSD(currentFrame);

        // Only stream if the startup sync completed and we are connected to the server, connection is checked in the function
        static float lastSentScale = NAN;
        static float lastSentThrust = NAN;
        
        bool HX711ValueChanged = (scaleReading != lastSentScale) || (thrustReading != lastSentThrust);
        if (timerTriggered || HX711ValueChanged) {
            lastSentScale = scaleReading;
            lastSentThrust = thrustReading;

            streamDataRow(currentFrame);

            if (printTimer >= printInterval) {
                printTimer = 0;
                String msg = 
                        String(currentFrame.timestamp) + "," +
                        String(currentFrame.servoAngle) + "," +
                        String(currentFrame.scaleReading, 4) + "," +
                        String(currentFrame.thrustReading, 4) + "," +
                        String(currentFrame.pt1Reading) + "," +
                        String(currentFrame.pt2Reading) + "\n";

                Serial.print(msg);
            }
        }
    }




    
}
    