/**********************************************************************
* File: ServoControl.h
* Author: Peter
* Description: 
*   Define Servo control functions
* 
**********************************************************************/
#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <Servo.h>
#include "PinConstants.h"

// extern Servo oServo;

void closeServo(Servo &servo);
void trickleServo(Servo &servo, int degree);
void fullFlowServo(Servo &servo);

#endif