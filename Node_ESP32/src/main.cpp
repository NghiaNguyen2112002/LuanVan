#include <Arduino.h>

#include "Lorawan.h"
#include "my_wifi.h"


const char* staName = "ESP32 Gateway";
const char* staPass = "123456789";

#define LORAWAN_BAUDRATE            115200

#define RX_0                        3
#define TX_0                        1
#define RX_1                        25 
#define TX_1                        26


String strSer;

void setup() {
  uint8_t cnt;

  Serial.begin(115200, SERIAL_8N1, RX_0, TX_0);


  cnt = 10;
  while(cnt-- > 0){
    Serial.println("Start Init Module!");
    delay(500);
  }

  // while(Serial.available() == 0){
  //   Serial.println("Enter something");  
  //   delay(500);
  // }

  // strSer = Serial.readString();

  WF_Init((char*)staName, (char*)staPass, NULL, NULL, 1);

  LRW_Init(RX_1, TX_1, LORAWAN_BAUDRATE);

  Serial.println("/************ BEGIN ************/");

}

void loop() {

  // if(Serial.available()){
  //   strSer = Serial.readString();

  //   Serial.print("ESPNow sending: "); Serial.println(strSer);
  //   WF_SendESPNOW((uint8_t*)strSer.c_str(), strSer.length());
  // }

  if(Serial1.available()){
    strSer = Serial1.readString();

    Serial.print("LORA: "); Serial.println(strSer);
  }

  if(Serial.available()){
    strSer = Serial.readString();
    Serial.print("Received: "); Serial.println(strSer);

    if(strSer.indexOf("AT") != -1){
      SendCMD(strSer);
    }
    else {
      Serial.print("MSG: "); Serial.println(LRW_SendMsg(1, 15, strSer));
    }
    
  }

}

