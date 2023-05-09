// Intialise library which communicates with RGB driver
// Functions accessible under 'nicla' namespace
#include "Nicla_System.h"


void main_app(void* happyfunct)
// not sure why it wants a void pointer other than to fit
// the type of a osThreadFunc_t
{
  while(1) {
    //run this code in a loop
    nicla::leds.setColor(green);  //turn green LED on
    osDelay(1000);                  //wait 1 second
    nicla::leds.setColor(off);    //turn all LEDs off
    osDelay(1000);                  //wait 1 second
  }
}



int main(void)
{
  //run this code once when Nicla Sense ME board turns on
  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
  osKernelInitialize();
  osThreadNew(main_app, NULL, NULL);
  osKernelStart();
  return 0;
}