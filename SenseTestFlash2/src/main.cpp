/* Example for blinking and sensor reading in two RTOS threads*/
//Include the relevant libraries
//Note that the BHY2 library requires the arduino BLE library to be
//available in the project as well


#include "niclaflash.h"

#include "Arduino.h"
#include "Nicla_System.h"
//#include "Arduino_BHY2.h"
#include "SPIFBlockDevice.h"
#include "MBRBlockDevice.h"

#include <stdint.h>


//This namespace allows use of time in simple formats
// such as 1s for 1 second
using namespace std::chrono_literals;

//declare the threads

rtos::Thread blinkthread(osPriorityAboveNormal);//heartbeat blink

/*
function to run in a thread and blink the onboard led
 */
void blink_app(void)
{
  while(1) {
    nicla::leds.setColor(green);                       //turn green LED on
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
    nicla::leds.setColor(off);                         //turn all LEDs off
    rtos::ThisThread::sleep_for(1s);                   //wait 1 second
  }
}


void setup(void)
{
  //run this code once when Nicla Sense ME board turns on
  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
  Serial.begin(115200);
  while(!Serial);
  Serial.println(String("log msg: start")); //example of sending a message to debug with serial


  NiclaFlash extflashmem;
  Serial.println(String(extflashmem.get_readaddress()));
  Serial.println(String(extflashmem.get_writeaddress()));
  //extflashmem.flashtest();
  //extflashmem.get_blockdevice().erase(FLASH_START_ADDRESS, ERASE_MIN_SIZE*1);
  extflashmem.writestring(String("Hello "));
  extflashmem.readchunktoserial(uint32_t(7));
  extflashmem.writestring(String("World\n"));
  extflashmem.readchunktoserial(uint32_t(6));

  Serial.println(String("log msg: flash hello world end"));

  blinkthread.start(blink_app);
  blinkthread.join(); //wait for blink to end (aka never return to main)
}


void loop(void){
  
}

