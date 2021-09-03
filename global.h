#ifndef GLOBAL_h
#define GLOBAL_h

#include "Arduino.h"

int debounce() { // Anti Debounce function

  volatile static long last_stateChange_event = 0; // When button is pushed, the last state change event is zero time since it's pushed
  volatile unsigned long stateChange_event = millis(); // Tracks the current time when the button is pushed

  if (stateChange_event - last_stateChange_event > 100) { // If's more than 100 ms since last state change, it accepting a new state change
    last_stateChange_event = stateChange_event; // Sets the last state change event to the current state change
    
    return HIGH;
    
  } else {
    //last_stateChange_event = stateChange_event;
    return LOW;
  }
}

#endif
