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
#include "ArduinoBLE.h"

//------------------------------------------------------
// Defines
//------------------------------------------------------
#define minAltitudeToDeployParachute 60000      // The parachute won't deploy if the rocket is lower than this value. (milimeters)
#define pressureAtSealevel 1013.25f             // pressure at sea level (hPa - hectopascal)

//------------------------------------------------------
// Globals (this probably should not exist)
//------------------------------------------------------
u_int64_t altitudevalue = 0; // In mm
bool parachuteDeploy = false; // false = parachute should deploy if it hasnt already

const char* sensing_service_uuid="181A"; // ID for enviromental sensing
const char* temp_characteristic_uuid="2A6E"; //ID for a temperature characteristic
const char* pressure_characteristic_uuid="2A6D"; //ID for a pressure characteristic
const char* altitude_characteristic_uuid="2A6C"; //ID for an elevation characteristic
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

BLEService sensing_service(sensing_service_uuid);
BLEShortCharacteristic temp_characteristic(temp_characteristic_uuid, BLERead | BLEIndicate);
BLEUnsignedIntCharacteristic pressure_characteristic(pressure_characteristic_uuid, BLERead | BLEIndicate);
BLEUnsignedIntCharacteristic altitude_characteristic(altitude_characteristic_uuid, BLERead | BLEIndicate);



// Declare the threads with priorities
rtos::Thread SensorUpdate(osPriorityNormal4);
rtos::Thread CheckApogee(osPriorityNormal3);   
//rtos::Thread LogSensorData(osPriorityRealtime);
rtos::Thread SensorPrint(osPriorityNormal2);
rtos::Thread Blink(osPriorityNormal1);

//------------------------------------------------------
// Functions
//------------------------------------------------------

int16_t float_to_int16(float input)
{
  input=input*100;//assume float is something like 12.34
  //Then trim to 16 bit
  if (input <-32768)
  {
    return -32768;
  } 
  else if (input >32767)
  {
    return 32767;
  }
  else
  {
    return (int16_t) round(input);
  }
}


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
  while (true)
  {
  rtos::ThisThread::sleep_for(200ms);                   //wait 500ms (Using chrono duration)
  BHY2.update();
  altitudevalue = calculateAltitude(pressure.value()); // ±25 cm
  temp_characteristic.writeValue(float_to_int16(temp.value()));
  pressure_characteristic.writeValue((uint32_t) (pressure.value()*100));
  altitude_characteristic.writeValue((uint32_t) altitudevalue);  
  }
  

}

/******************************************************************************
 * @brief Determines apogee of rocket when alitude vs time slope goes negative.
 *        Checks every 100ms.
 ******************************************************************************/
void checkApogee(void) {
  u_int64_t lastAltitude = 0;
  while(true) {
    if(altitudevalue > minAltitudeToDeployParachute && !parachuteDeploy) {
      if(altitudevalue < lastAltitude) {
        parachuteDeploy = true;
        rtos::ThisThread::sleep_for(1s);
        if(parachuteDeploy != true) {
          parachuteDeploy = true;
        }
        // turn off this task?
      }
      lastAltitude = altitudevalue;
     
    }
     rtos::ThisThread::sleep_for(500ms);
  }
}

/******************************************************************************
 * @brief Prints sensor data to serial monitor every second.
 ******************************************************************************/
void sensorPrint(void) {
  while(true) {
    rtos::ThisThread::sleep_for(4s);                   //wait 500ms (Using chrono duration)
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

if (!BLE.begin()){
    Serial.println("Starting BLE failed");
    while(1);
  }
  // Intialize 
  BHY2.begin();
  accel.begin();
  gyro.begin();
  temp.begin();
  rotation.begin();
  pressure.begin();
  nicla::leds.setColor(green);
  BLE.setLocalName("Nicla Sense Avionics 2");
  sensing_service.addCharacteristic(temp_characteristic);
  sensing_service.addCharacteristic(pressure_characteristic);
  sensing_service.addCharacteristic(altitude_characteristic);
  BLE.addService(sensing_service);

  temp_characteristic.writeValue(0);
  pressure_characteristic.writeValue(0);
  altitude_characteristic.writeValue(0);

  BLE.advertise();
  // Make threads run associated functions
  SensorUpdate.start(updateSensorData);
  CheckApogee.start(checkApogee); 
  //LogSensorData.start();
  SensorPrint.start(sensorPrint);
  //Blink.start(blink);




}

void loop(void) {
  // DO NOT PUT ANYTHING HERE!
    nicla::leds.setColor(red);                       //turn green LED on
    rtos::ThisThread::sleep_for(1s);                 //wait 1 second
    nicla::leds.setColor(off);                       //turn all LEDs off
    rtos::ThisThread::sleep_for(1s);                 //wait 1 second
}