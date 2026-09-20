/**********************************************************************
* File: PinConstants.h
* Author: Peter
* Description: 
*   Define pins
* 
**********************************************************************/
#include <ADS1256.h>

#ifndef PIN_CONSTANTS_H
#define PIN_CONSTANTS_H
//TODO: define and change pins

#define USE_TEST_PINS

// OFFICAL PINS
namespace Pins 
{
    static const unsigned char IGNITER_PIN = 1;
    static const unsigned char SERVO_PIN = 4;
    static const unsigned char IGNITER_SWITCH_PIN = 31;
    static const unsigned char TRICKLE_SWITCH_PIN = 30;
    static const unsigned char FULL_FLOW_SWITCH_PIN = 29;
    static const unsigned char RESET_SWITCH_PIN = 28;
    static const unsigned char TANKLOADCELL_DOUT_PIN = 26;
    static const unsigned char TANKLOADCELL_SCK_PIN = 27;
    // static const unsigned char FFF_TIMER_PIN_PIN = 28;
    // static const unsigned char FFF_TIMER_PIN_POUT = 29;
    static const unsigned char PT_OXVALVE_PIN = 5;
    static const unsigned char PT_COMB_PIN = 6;
    // static const unsigned char READY_PIN = 0;
    static const unsigned char LED_PIN = 32;
    static const unsigned char ADS_DRDY_PIN = 24;
    static const unsigned char ADS_RESET_PIN = ADS1256::PIN_UNUSED;
    static const unsigned char ADS_SYNC_PIN = 25;
    static const unsigned char ADS_CS_PIN = 10;
}

//TESTBOARD PINS
namespace TestPins 
{
    static const unsigned char IGNITER_PIN = 19;
    // static const unsigned char IGNITER_PIN = 20;
    // static const unsigned char IGNITER_PIN = 21;
    static const unsigned char SERVO_PIN = 4;
    static const unsigned char IGNITER_SWITCH_PIN = 31;
    static const unsigned char TRICKLE_SWITCH_PIN = 30;
    static const unsigned char FULL_FLOW_SWITCH_PIN = 29;
    static const unsigned char RESET_SWITCH_PIN = 28;
    static const unsigned char TANKLOADCELL_DOUT_PIN = 26;
    static const unsigned char TANKLOADCELL_SCK_PIN = 27;
    // static const unsigned char FFF_TIMER_PIN_PIN = 28;
    // static const unsigned char FFF_TIMER_PIN_POUT = 29;
    static const unsigned char PT_ONE_PIN = 18;
    static const unsigned char PT_TWO_PIN = 22;
    // static const unsigned char READY_PIN = 0;
    static const unsigned char LED_PIN = 32;
    // static const unsigned char ADS_DRDY_PIN = 24;
    // static const unsigned char ADS_RESET_PIN = ADS1256::PIN_UNUSED;
    // static const unsigned char ADS_SYNC_PIN = 25;
    // static const unsigned char ADS_CS_PIN = 10;
    static const unsigned char SCALE_DOUT_PIN = 10;
    static const unsigned char SCALE_SCK_PIN = 12;
    static const unsigned char THRUST_DOUT_PIN = 11;
    static const unsigned char THRUST_SCK_PIN = 13;
}

#ifdef USE_TEST_PINS
    #define ActivePins TestPins
#else
    #define ActivePins Pins
#endif

#endif