/**********************************************************************
 * File: ServoControl.cpp
 * Author: Peter
 * Description:
 *  Implement Servo control functions
 *
 **********************************************************************/

#include "ServoControl.h"

#include <Arduino.h>
#include <Servo.h>

void closeServo(Servo &servo)
{
    servo.write(180);
    // Serial.println("ServoControl: Closed (180°)");
}

void trickleServo(Servo &servo, int angle)
{
    if (angle < 0 || angle > 180)
    {
        Serial.print("ERROR: Invalid servo angle value = ");
        Serial.print(angle);
        Serial.println("°");
        return;
    }
    servo.write(angle);
    // Serial.print("ServoControl: Moved to trickle angle(");
    // Serial.print(angle);
    // Serial.println("°)");
}

void fullFlowServo(Servo &servo)
{
    servo.write(90);
    // Serial.println("ServoControl: Full Open (90°)");
}