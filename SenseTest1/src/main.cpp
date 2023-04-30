

#include <Arduino.h>
// Intialise library which communicates with RGB driver
// Functions accessible under 'nicla' namespace
#include "Nicla_System.h"       
int main(void) {
  //run this code once when Nicla Sense ME board turns on
  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
  while(1) {
    //run this code in a loop
    nicla::leds.setColor(green);  //turn green LED on
    delay(1000);                  //wait 1 second
    nicla::leds.setColor(off);    //turn all LEDs off
    delay(1000);                  //wait 1 second
  }
 return 0;  
}







