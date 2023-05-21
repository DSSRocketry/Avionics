/* Example for blinking and sensor reading in two RTOS threads*/
//Include the relevant libraries
//Note that the BHY2 library requires the arduino BLE library to be
//available in the project as well
#include "Arduino.h"
#include "Nicla_System.h"
#include "Arduino_BHY2.h"

//This namespace allows use of time in simple formats
// such as 1s for 1 second
using namespace std::chrono_literals;


//declare the sensors
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
Sensor pressure(SENSOR_ID_BARO);
SensorQuaternion rotation(SENSOR_ID_RV);

//declare the threads

rtos::Thread blinkthread(osPriorityAboveNormal);//heartbeat blink is wanted
rtos::Thread sensorupdatethread;//not actually used currently
rtos::Thread sensorprintthread;//sensor updating will happily consume all time it seems
//some testing remains with the thread priority. below normal seems to cause threads to not run

/**function to run in a thread and blink the onboard led
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

/**function to run in a thread and update sensor data.
 * You want to update it constantly
 */
void sensorupdate_app(void){
  BHY2.update();

}




/**function to run in a thread and print sensor data to the serial port
 * you only want to print every second
 */
void sensorprint_app(void)
{

  while(1) {
    BHY2.update();
    Serial.println(String("acceleration: ") + accel.toString());
    Serial.println(String("gyroscope: ") + gyro.toString());
    Serial.println(String("temperature: ") + String(temp.value(),3));
    Serial.println(String("gas: ") + String(gas.value(),3));
    Serial.println(String("pressure: ") + String(pressure.value(),3));
    Serial.println(String("rotation: ") + rotation.toString());
    Serial.println(millis());
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
  Serial.println(String("log msg 1")); //example of sending a message to debug with serial
  BHY2.begin();

  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();
  pressure.begin();

  blinkthread.start(blink_app);
  //sensorupdatethread.start(sensorupdate_app);
  sensorprintthread.start(sensorprint_app);
  blinkthread.join(); //wait for blink to end (aka never return to main)
}


void loop(void){
  
}

