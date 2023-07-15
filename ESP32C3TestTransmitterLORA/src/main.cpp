#include <Arduino.h>

HardwareSerial lora = Serial1;
 
long increment_tracker1=0; 
void setup()
{
  Serial.begin(115200);
  lora.begin(115200);
  //pinMode(pot, INPUT);
  delay(500);
  //Serial.println(String("In setup"));
  //String cmd = "AT+SEND=0,"+String(potval.length()) +","+ String(val)+"\r";
  String cmd = "AT+ADDRESS=1\r\n";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  //delay(500);
  cmd = "AT+NETWORKID=1\r\n";
  //Serial.println("AT+SEND=0,3,val");
  lora.println(cmd);
  delay(50);
  while(lora.available()){
    Serial.write(lora.read());
  }
  Serial.println(String("Done"));


}
 
void loop()
{
  increment_tracker1=increment_tracker1+1;
  String msg="Hello World "+String(increment_tracker1);
  String cmd;
  cmd = "AT+SEND=2,"+String(msg.length()) +","+ msg+"\r\n";
  lora.println(cmd);
  //Serial.println("Sent Message");
  Serial.println(cmd);
  // delay(50);
  // while(lora.available()){
  //   Serial.write(lora.read());
  // }
  delay(1000);
}

