#include <SoftwareSerial.h>
 
SoftwareSerial lora(2,3);
 
int LED = 5;
//String inString = "";    // string to hold input
int val = 0;
 
void setup()
{
  Serial.begin(115200);
  lora.begin(115200);
  //pinMode(LED, OUTPUT);
 
}
 
void loop()
{
  //char ch;
  String inString;
  while (lora.available())
  {
    if(lora.available()){
    inString += String(char(lora.read()));
    }
  }
  if(inString.length()>0)
  {
    //Serial.println(inString);
    String potval;
    potval= inString.substring(9,12);
    Serial.println(potval);
    //analogWrite(LED,potval.toInt());
  }
  //delay(100);
}