#include <Arduino.h>

// Intialise library which communicates with RGB driver
// Functions accessible under 'nicla' namespace
#include "Nicla_System.h"       


void setup() {
  //run this code once when Nicla Sense ME board turns on
  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
}

void loop() {
  //run this code in a loop
  nicla::leds.setColor(green);  //turn green LED on
  delay(1000);                  //wait 1 second
  nicla::leds.setColor(off);    //turn all LEDs off
  delay(1000);                  //wait 1 second
}