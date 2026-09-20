/**********************************************************************
* File: StateMachine.cpp
* Author: Peter
* Description: 
*   Control system states and transistions
* 
**********************************************************************/
#include "StateMachine.h"
#include "PinConstants.h"
#include "IgniterControl.h"
#include "ServoControl.h"

#include <Arduino.h>
#include <Servo.h>

// #define SERIAL_OVERRIDE

extern SystemStatus systemStatus;
extern Servo oServo;
extern unsigned long countdownStartTime;
extern const unsigned long COUNTDOWN_LIMIT_MS;

int servoAngle = 180;
// change this  to initial trickle angle
int servoTrickleAngle = 154; //26 degrees from closing


void initStateMachine() 
{




}

void updateStateMachine()
{
    // Active Low
    bool igniterSwitchPressed = (digitalRead(ActivePins::IGNITER_SWITCH_PIN) == LOW);
    bool trickleSwitchPressed = (digitalRead(ActivePins::TRICKLE_SWITCH_PIN) == LOW);
    bool fullFlowSwitchPressed = (digitalRead(ActivePins::FULL_FLOW_SWITCH_PIN) == LOW);
    bool resetSwitchPressed = (digitalRead(ActivePins::RESET_SWITCH_PIN) == LOW);
    static bool previousIgniterState = false;
    static bool previousTrickleState = false;
    static bool previousFullFlowState = false;
    static bool previousResetState = false;
    
    static elapsedMillis resetTimer;
    
    if (igniterSwitchPressed) 
    {
        onIgniter();
        systemStatus.igniterOn = true;

    }
    else
    {
        offIgniter();
        systemStatus.igniterOn = false;
    }

    if (igniterSwitchPressed) 
    {
        if (!previousIgniterState) {
            
            Serial.println("Igniter Switch Pressed.");
        }
    }
    else
    {
        if (previousIgniterState) {
            
            Serial.println("Igniter Switch Released.");
            
        }
    }

    previousIgniterState = igniterSwitchPressed;

    if (trickleSwitchPressed) {
        if (!previousTrickleState) {
            Serial.println("Trickle Switch Pressed.");
        }
    }
    else
    {
        if (previousTrickleState) {
            Serial.println("Trickle Switch Released.");
        }
    }

    previousTrickleState = trickleSwitchPressed;

    if (fullFlowSwitchPressed) {
        if (!previousFullFlowState) {
            Serial.println("Full Flow Switch Pressed.");
        }
    }
    else
    {
        if (previousFullFlowState) {
            Serial.println("Full Flow Switch Released.");
        }
    }

    previousFullFlowState = fullFlowSwitchPressed;

    if (resetSwitchPressed) {
        if (!previousResetState) {
            Serial.println("Reset Switch Pressed.");
        }
    }
    else
    {
        if (previousResetState) {
            Serial.println("Reset Switch Released.");
        }
    }

    previousResetState = resetSwitchPressed;

    // reset switch
    if (resetSwitchPressed) {
        if (resetTimer >= 1000) {
            if (servoAngle <= 90) {
                servoAngle = 170;
            }
            else {
                servoAngle -= 3;
            }
            resetTimer = 0;
        }
    }
    else {
        resetTimer = 0;
    }

    // if (!trickleSwitchPressed)
    // {
    //     servoAngle = 170;
    // }

    #ifdef SERIAL_OVERRIDE
    static bool serialTrickleSwitch = false;
    static bool serialFullFlowSwitch = false;
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command == 't') {
            serialTrickleSwitch = !serialTrickleSwitch;
        } 

        if (command == 'f') {
            serialFullFlowSwitch = !serialFullFlowSwitch;
        }

        if (command == 'a') {
            servoAngle -= 1;
            if (servoAngle < 90) {
                servoAngle = 90;
            }

        }

        if (command == 'd') {
            servoAngle += 1;
            if (servoAngle > 180) {
                servoAngle = 180;
            }

        }

        if (command == 'c') {
            servoAngle = 180;
        }
    }

    if (serialFullFlowSwitch) {
        fullFlowServo(oServo);
    }
    else if (serialTrickleSwitch) {
        trickleServo(oServo, servoAngle);
    }
    else {
        closeServo(oServo);
    }
    #endif

    switch (systemStatus.currentState)
    {
        // TODO: Add idle to ready state check
        case IDLE:

            systemStatus.currentState = READY;
            break;

        case READY:

            if (trickleSwitchPressed)
            {
                systemStatus.currentState = VALVE_TRICKLE;
                trickleServo(oServo, servoTrickleAngle);
                servoAngle = servoTrickleAngle;
            }
            break;

        case VALVE_TRICKLE:
            // start timers here
            // if (!systemStatus.hardwareTimerOn)
            // {
            //     startHardwareTimer();
            // }
            // updateHardwareTimer();

            // if (systemStatus.timeOut)
            // {
            //     resetStateMachine();

            // }


            if (fullFlowSwitchPressed)
            {
                
                fullFlowServo(oServo);
                servoAngle = 90;
            }
            else if (trickleSwitchPressed)
            {
                trickleServo(oServo, servoTrickleAngle);
                servoAngle = servoTrickleAngle;
            } 
            else
            {
                closeServo(oServo);
                servoAngle = 180;
            }
            break;
        

    }
}

void resetStateMachine() {
    Serial.println("State machine resetting...");
    closeServo(oServo);
    // resetHardwareTimer();
    // systemStatus.timeOut = false;
    systemStatus.currentState = IDLE;
}

void startHardwareTimer() {
    countdownStartTime = millis();
    systemStatus.hardwareTimerOn = true;
}

void resetHardwareTimer() {
    countdownStartTime = 0;
    systemStatus.hardwareTimerOn = false;
}

void updateHardwareTimer() {
    if (!systemStatus.hardwareTimerOn) return;

    unsigned long timeElapsed = millis() - countdownStartTime;
    Serial.print("T-Plus: ");
    Serial.println(timeElapsed);

    if (timeElapsed >= COUNTDOWN_LIMIT_MS) {
        systemStatus.timeOut = true;
        Serial.println("Hardware timer limit reached.");
    }
}
