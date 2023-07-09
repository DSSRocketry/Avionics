#include <Arduino.h>
 
int LED = 5;
//String inString = "";    // string to hold input
int val = 0;

HardwareSerial lora = Serial1;


void setup()
{
  Serial.begin(115200);
  lora.begin(115200);
  //pinMode(pot, INPUT);
  delay(500);
  //Serial.println(String("In setup"));
  //String cmd = "AT+SEND=0,"+String(potval.length()) +","+ String(val)+"\r";
  String cmd = "AT+ADDRESS=2\r\n";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  delay(500);
  cmd = "AT+NETWORKID=1\r\n";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  delay(500);
  while(lora.available()){
    //Serial.println(String("In available"));
    Serial.write(lora.read());
  }
  Serial.println(String("Done"));
 
}
 
void loop()
{
  // //char ch;
  // String inString;
  // while (lora.available())
  // {
  //   if(lora.available()){
  //   inString += String(char(lora.read()));
  //   }
  // }
  // if(inString.length()>0)
  // {
  //   //Serial.println(inString);
  //   Serial.println(inString);
  //   //analogWrite(LED,potval.toInt());
  // }
  delay(100);
}

