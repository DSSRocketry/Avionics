// Intialise library which communicates with RGB driver
// Functions accessible under 'nicla' namespace
#include "Nicla_System.h"
using namespace std::chrono_literals;

rtos::Thread blinkthread;

void blink_app(void)

{
  while(1) {
    Serial.println(String("log msg 2"));
    //run this code in a loop
    nicla::leds.setColor(green);                       //turn green LED on
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
    nicla::leds.setColor(off);                         //turn all LEDs off
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
  }
}



int main(void)
{
  //run this code once when Nicla Sense ME board turns on
  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
  Serial.begin(115200);
  while(!Serial);
  Serial.println(String("log msg 1"));
  blinkthread.start(blink_app);
  while(1);
  return 0;
}