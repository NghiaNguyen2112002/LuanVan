#include "my_wifi.h"


const uint8_t peerMac[] = {0x4C, 0x75, 0x25, 0xCF, 0x3F, 0x30};

/* This ESP MAC Address 94:E6:86:2C:BD:4C */
uint8_t myMac[6];
esp_now_peer_info_t peerInfor;


/************************************************************
*                   LOW LEVEL FUNCTION                      *
*************************************************************/
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
    Serial.println("Sent successfully!");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){
    Serial.printf("EspNow Received: %s\n", data);
}





/************************************************************
*                   HIGH LEVEL FUNCTION                     *
*************************************************************/
void WF_Init(char* staName, char* staPass, char* apName, char* apPass, uint8_t isEspNow){
    WiFi.mode(WIFI_AP_STA);

    ESP_ERROR_CHECK(esp_efuse_mac_get_default(myMac));
    Serial.print("MAC Address: ");
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", myMac[0], myMac[1], myMac[2], myMac[3], myMac[4], myMac[5]);
    
    if(staName != NULL){
        Serial.printf("Connecting to: %s %s\n", staName, staPass);
        while(WiFi.status() != WL_CONNECTED){
            WiFi.begin(staName, staPass);
            Serial.print(".");
            delay(3000);
        }
    }

    if(apName != NULL){        
        Serial.printf("Begin AP to: %s %s\n", apName, apPass);
        WiFi.softAP(apName, apPass);
    }

    if(isEspNow){
        Serial.println("Begin espNow!");

        if (esp_now_init() != 0) {
            Serial.println("Error initializing ESP-NOW");
            esp_restart();
        }

        /* Set role */

        /* Register peer */
        memcpy(peerInfor.peer_addr, peerMac, 6);
        peerInfor.channel = 0;
        peerInfor.encrypt = false;
        

        esp_now_add_peer(&peerInfor);

        /* Assign callback function */
        esp_now_register_send_cb(OnDataSent);
        esp_now_register_recv_cb(OnDataRecv);

    }



}


void WF_Connect(void);
void WF_Disconnect(void);
uint8_t WF_IsConnected(void);



void WF_SendESPNOW(uint8_t* data, uint8_t length){
    esp_now_send(peerMac, data, length);
}











