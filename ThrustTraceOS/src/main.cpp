// ThrustTraceOS V1.0
// OS for a nickla sense board which will go onto a future DSS rocket

// Notes:
// 1. One cpp file for simplicity. 
// 2. UNTESTED!!! Nothing here has been tested at all.


//------------------------------------------------------
// Libraries
//------------------------------------------------------

#include "Arduino.h"
#include "Nicla_System.h"
#include "Arduino_BHY2.h"

//------------------------------------------------------
// Defines
//------------------------------------------------------
#define minAltitudeToDeployParachute 60000      // The parachute won't deploy if the rocket is lower than this value. (milimeters)
#define pressureAtSealevel 1013.25f             // pressure at sea level (hPa - hectopascal)

//------------------------------------------------------
// Globals (this probably should not exist)
//------------------------------------------------------
u_int64_t altitude = 0; // In mm
bool parachuteDeploy = false; // false = parachute should deploy if it hasnt already

//------------------------------------------------------
// Namespaces
//------------------------------------------------------
using namespace std::chrono_literals; // Allows use of time in simple formats

//------------------------------------------------------
// Declarations
//------------------------------------------------------

// Declare the sensor objects
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor pressure(SENSOR_ID_BARO);
SensorQuaternion rotation(SENSOR_ID_RV);

// Declare the threads with priorities
rtos::Thread SensorUpdate(osPriorityRealtime7);
rtos::Thread CheckApogee(osPriorityRealtime6);   
//rtos::Thread LogSensorData(osPriorityRealtime);
rtos::Thread SensorPrint(osPriorityLow);
rtos::Thread Blink(osPriorityLow);

//------------------------------------------------------
// Functions
//------------------------------------------------------

/******************************************************************************
 * @brief Calculates the alitude from a given pressure.
 * @param pressure pressure in hPa.
 * @return the altitude of the pressure sensor in milimeters.
 ******************************************************************************/
u_int64_t calculateAltitude(double pressure) {
  return 100 * 44330 * (1 - pow(pressure/pressureAtSealevel,1/5.255));
}

//------------------------------------------------------
// Threads (kinda)
//------------------------------------------------------

/******************************************************************************
 * @brief Updates the sensor data (pressure, gyro, accel, temp, mag).
 *        Calculates altitude from pressure.
 ******************************************************************************/
void updateSensorData(void) {
  BHY2.update();
  altitude = calculateAltitude(pressure.value()); // ±25 cm
}

/******************************************************************************
 * @brief Determines apogee of rocket when alitude vs time slope goes negative.
 *        Checks every 100ms.
 ******************************************************************************/
void checkApogee(void) {
  u_int64_t lastAltitude = 0;
  while(true) {
    if(altitude > minAltitudeToDeployParachute && !parachuteDeploy) {
      if(altitude < lastAltitude) {
        parachuteDeploy = true;
        rtos::ThisThread::sleep_for(1s);
        if(parachuteDeploy != true) {
          parachuteDeploy = true;
        }
        // turn off this task?
      }
      lastAltitude = altitude;
      rtos::ThisThread::sleep_for(100ms);
    }    
  }
}

/******************************************************************************
 * @brief Prints sensor data to serial monitor every second.
 ******************************************************************************/
void sensorPrint(void) {
  while(true) {
    rtos::ThisThread::sleep_for(1s);                   //wait 500ms (Using chrono duration)
    Serial.println(String("acceleration: ") + accel.toString());
    Serial.println(String("gyroscope: ") + gyro.toString());
    Serial.println(String("temperature: ") + String(temp.value(),3));
    Serial.println(String("pressure: ") + String(pressure.value(),3));
    Serial.println(String("rotation: ") + rotation.toString());
    Serial.println(millis());
  }
}

/******************************************************************************
 * @brief Blinks onboard led every second.
 ******************************************************************************/
void blink(void) {
  while(true) {
    nicla::leds.setColor(red);                       //turn green LED on
    rtos::ThisThread::sleep_for(1s);                 //wait 1 second
    nicla::leds.setColor(off);                       //turn all LEDs off
    rtos::ThisThread::sleep_for(1s);                 //wait 1 second
  }
}

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
  rotation.begin();
  pressure.begin();

  // Make threads run associated functions
  SensorUpdate.start(updateSensorData);
  CheckApogee.start(checkApogee); 
  //LogSensorData.start();
  SensorPrint.start(sensorPrint);
  Blink.start(blink);
}

void loop(void) {
  // DO NOT PUT ANYTHING HERE!
}