#include "Lorawan.h"

static String loraMess;

/************************************************************
*                   LOW LEVEL FUNCTION                      *
*************************************************************/
void SendCMD(String cmd){
    Serial1.print(cmd);
}

String WaitMsg(uint16_t timeOut){
    uint32_t timeCounter = millis();

    loraMess = "";

    while((millis() - timeCounter) < timeOut){
        if(Serial1.available()){                      
            // loraMess = Serial1.readString();
            loraMess += Serial1.readString();

        }
    }

    return loraMess;
}



/************************************************************
*                   HIGH LEVEL FUNCTION                     *
*************************************************************/
void LRW_Init(uint8_t RX, uint8_t TX, uint32_t baudrate){
    Serial1.begin(baudrate, SERIAL_8N1, RX, TX);

    delay(2000);

    while(LRW_IsDeviceConnected() != 1);
    Serial.println("LORAWAN Device connected");


    // LRW_StopJoin();

    SendCMD("AT+CRESTORE\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    SendCMD("AT+ILOGLVL=0\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    SendCMD("AT+CSAVE\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    SendCMD("AT+IREBOOT=0\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    delay(1000);
    
    LRW_SetClass(CLASS_TEST);

    SendCMD("AT+CDATARATE=0\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    LRW_ConfigABP(LRW_DEVADDR, LRW_APP_S_KEY, LRW_NWK_S_KEY, "2");

    // LRW_ConfigOTTA(LRW_DEVEUI, LRW_APPEUI, LRW_APPKEY, "2");




    SendCMD("AT+CWORKMODE=2\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    delay(1000);

    // LRW_SetRxWindow("923300000");

    LRW_SetFreqMask("0001");

    // LRW_StartJoin();


}


uint8_t LRW_IsDeviceConnected(void){
    SendCMD("AT+CGMI?\r\n");

    loraMess = WaitMsg(500);

    if(loraMess.indexOf("OK") != -1){
        return 1;
    }
    else {
        return 0;
    }
}

uint8_t LRW_IsJoined(void){
    SendCMD("AT+CSTATUS?\r\n");

    loraMess = WaitMsg(500);

    if(loraMess.indexOf("CSTATUS") != -1){
        if( (loraMess.indexOf("03") != -1) 
            || (loraMess.indexOf("07") != -1) 
            || (loraMess.indexOf("08") != -1) 
            ){
                
                return 1;
            }
        else {
            return 0;
        }
    }

    return 0;
}

void LRW_ConfigOTTA(String deviceEUI, String appEUI, String appKey, String uldlMode){
    SendCMD("AT+CJOINMODE=0\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    loraMess = "AT+CDEVEUI=" + deviceEUI + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));


    loraMess = "AT+CAPPEUI=" + appEUI + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));


    loraMess = "AT+CAPPKEY=" + appKey + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));


    loraMess = "AT+CULDLMODE=" + uldlMode + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));
}

void LRW_ConfigABP(String deviceAddr, String appSKey, String netSKey, String uldlMode){
    SendCMD("AT+CJOINMODE=1\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    loraMess = "AT+CDEVADDR=" + deviceAddr + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    loraMess = "AT+CAPPSKEY=" + appSKey + "\r\n";
    SendCMD(loraMess);  
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    loraMess = "AT+CNWKSKEY=" + netSKey + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

    loraMess = "AT+CULDLMODE=" + uldlMode + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));
}

void LRW_SetClass(String className){
    loraMess = "AT+CCLASS=" + className + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

}

void LRW_SetRxWindow(String freq){
    loraMess = "AT+CRXP=0,8," + freq + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(loraMess = WaitMsg(500));

    // while(loraMess.indexOf("CME") != -1){
    //     loraMess = "AT+CRXP=0,8," + freq + "\r\n";
    //     SendCMD(loraMess);
    //     Serial.print("LORA:"); Serial.println(loraMess = WaitMsg(500));
    //     delay(10000);
    // }
}

void LRW_SetFreqMask(String mask){
    loraMess = "AT+CFREQBANDMASK=" + mask + "\r\n";
    SendCMD(loraMess);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

}

void LRW_StartJoin(void){
    SendCMD("AT+CJOIN=1,0,10,8\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

}

void LRW_StopJoin(void){
    SendCMD("AT+CJOIN=0,0,10,8\r\n");
    Serial.print("LORA:"); Serial.println(WaitMsg(500));

}

String LRW_SendMsg(uint8_t confirm, uint8_t nbTrials, String msg){
    String encodedData = LRW_EncodeMsg(msg);
    String cmd = "AT+DTRX=" + String(confirm) + ',' + String(nbTrials) + ',' +
                 String(encodedData.length()) + ',' + encodedData + "\r\n";

    SendCMD(cmd);
    Serial.print("LORA:"); Serial.println(WaitMsg(500));


    return cmd;
}

String LRW_ReceiveMsg(void){
    String restr = WaitMsg(2000);
    if (restr.indexOf("OK+RECV:") != -1 && restr.indexOf("02,00,00") == -1) {
        String data = restr.substring(restr.indexOf("OK+RECV:") + 17, -2);
        // return decodeMsg(data);
        return data;
    } else {
        return "";
    }
}

/* String to Hex String */
String LRW_EncodeMsg(String msg){
    char buff[msg.length() + 1];
    char tempBuff[(msg.length() + 1) * 2];
    char* p;
    uint16_t i;

    msg.toCharArray(buff, msg.length() + 1);

    i = 0;
    for(p = buff; *p; p++){
        sprintf( (char*) (tempBuff + i), "%02x", *p);
        i += 2;
    }

    return String(tempBuff);

}

/* Hex String to String */
String LRW_DecodeMsg(String msg){
    

    if(msg.length() & 1 != 0){
        return msg;
    }



}