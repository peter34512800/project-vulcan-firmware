/**********************************************************************
 * File: StateMachine.cpp
 * Author: Peter
 * Description:
 *  Define system states and functions for the state machine.
 *
 * States:
 *   IDLE                            - System default state
 *   READY                           - Ready state for test
 *   VALVE_TRICKLE                   - Valve opening trickle increment operation
 *   VALVE_FULL_OPEN                 - Valve full open state and start system end timer
 *   TIMER_END                       - Operation end by timer
 *   EMERGENCY_STOP                  - Emergency stop override
 *   ERROR                           - Fault condition
 *
 **********************************************************************/

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

enum State
{
    IDLE,
    READY,
    VALVE_TRICKLE
};

struct SystemStatus
{
    State currentState;
    bool igniterOn = false;
    bool sdInitialized = false;
    bool logDataSync = false;
    bool hardwareTimerOn = false;
    bool timeOut = false;

    SystemStatus() : currentState(IDLE) {}

    SystemStatus(State initialState) : currentState(initialState) {}
};

void initStateMachine();
void updateStateMachine();
void resetStateMachine();
void startHardwareTimer();
void resetHardwareTimer();
void updateHardwareTimer();


#endif