// Example for blinking and sensor reading in two RTOS threads
// Include the relevant libraries
// Note that the BHY2 library requires the arduino BLE library to be available in the project as well

#include "Arduino.h"
#include "Nicla_System.h"
#include "Arduino_BHY2.h"

// This namespace allows use of time in simple formats
using namespace std::chrono_literals;

// Declare the sensor objects
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor gas(SENSOR_ID_GAS);
Sensor pressure(SENSOR_ID_BARO);
SensorQuaternion rotation(SENSOR_ID_RV);

// Declare the threads with priorities
rtos::Thread blinkthread(osPriorityNormal);//heartbeat blink is wanted
rtos::Thread sensorprintthread(osPriorityRealtime);//sensor updating will happily consume all time it seems

// Blink led
void blink(void) {
  while(1) {
    nicla::leds.setColor(red);                       //turn green LED on
    rtos::ThisThread::sleep_for(1s);                 //wait 1 second
    nicla::leds.setColor(off);                       //turn all LEDs off
    rtos::ThisThread::sleep_for(1s);                 //wait 1 second
  }
}

// Update sensor data and print to serial monitor
void sensorprint(void) {
  while(1) {
    rtos::ThisThread::sleep_for(500ms);                   //wait 500ms (Using chrono duration)
    BHY2.update();
    Serial.println(String("acceleration: ") + accel.toString());
    Serial.println(String("gyroscope: ") + gyro.toString());
    Serial.println(String("temperature: ") + String(temp.value(),3));
    Serial.println(String("gas: ") + String(gas.value(),3));
    Serial.println(String("pressure: ") + String(pressure.value(),3));
    Serial.println(String("rotation: ") + rotation.toString());
    
    // 
    Serial.println(millis());
  }
}

// setup() runs once on the first power up of the board.
void setup(void) {

  nicla::begin();               // initialise library
  nicla::leds.begin();          // Start I2C connection
  Serial.begin(115200);
  while(!Serial);
  Serial.println(String("log msg 1")); //example of sending a message to debug with serial

  // Intialize 
  BHY2.begin();
  accel.begin();
  gyro.begin();
  temp.begin();
  gas.begin();
  rotation.begin();
  pressure.begin();

  // Make threads run associated functions
  blinkthread.start(blink);
  sensorprintthread.start(sensorprint);
}

void loop(void) {
  // DO NOT PUT ANYTHING HERE!
}