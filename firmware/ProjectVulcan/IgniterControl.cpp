/**********************************************************************
 * File: Igniter.cpp
 * Author: Peter
 * Description:
 *  Implement Igniter control functions
 *
 **********************************************************************/

#include <Arduino.h>

#include "IgniterControl.h"
#include "PinConstants.h"

//TODO: Change Igniter to high when active low and switch it to LOW on HIGH (SWITCH CONTROL)
// Turns Igniter pin on high for 5 seconds then back to low
void burnIgniter()
{
    // Serial.println();
    digitalWrite(ActivePins::IGNITER_PIN, HIGH);
    Serial.println("IgniterControl: IGNITER_PIN HIGH");
    delay(5000);
    digitalWrite(ActivePins::IGNITER_PIN, LOW);
    Serial.println("IgniterControl: IGNITER_PIN LOW");
}

//switch on Igniter
void onIgniter() 
{
    digitalWrite(ActivePins::IGNITER_PIN, HIGH);
    digitalWrite(ActivePins::LED_PIN, HIGH);
    // if (digitalRead(ActivePins::IGNITER_PIN) == HIGH) {
    //     Serial.println("Igniter pin is HIGH");
    // } else {
    //     Serial.println("Igniter pin is LOW");
    // }
    // Serial.println("IgniterControl: IGNITER_PIN HIGH");
}

void offIgniter()
{
    digitalWrite(ActivePins::IGNITER_PIN, LOW);
    digitalWrite(ActivePins::LED_PIN, LOW);
    // if (digitalRead(ActivePins::IGNITER_PIN) == HIGH) {
    //     Serial.println("Igniter pin is HIGH");
    // } else {
    //     Serial.println("Igniter pin is LOW");
    // }
    // Serial.println("IgniterControl: IGNITER_PIN LOW");
}
