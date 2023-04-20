/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini

   I extended this to be able to experiment with the Hoermann packets and pick out what I needed.
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
const BLEAddress garagedooraddr("DE:AD:BA:BE:55:55");

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.getAddress() == garagedooraddr) {       
        //Serial.printf("Advertised Device: %s %d\n", advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getPayloadLength());
        uint8_t *payload = advertisedDevice.getPayload();       
        for (size_t i = 0; i < advertisedDevice.getPayloadLength();) {
          int field_len = payload[i];
          int field_type = payload[i+1];
          if (field_type == ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE) {
            uint8_t *data = payload + i + 2;
            uint16_t manufacturer = ((uint16_t*)data)[0]; // 0x07B4 - Hoermann
            if (manufacturer == 0x07B4 && field_len == 20) {
              uint8_t door_state = data[2]; // 0x13 open, 0x14 closed, ....
              uint8_t position = data[3] / 2; // 0..200                       
              char* pHex = BLEUtils::buildHexData(nullptr, payload + i + 2, field_len - 2);
              Serial.println(pHex);
              Serial.printf("%04X %s %d%%\n", manufacturer, (door_state == 0x14 ? "closed" : "open"), position/2);
          	  free(pHex);
            }
          }      
          i += field_len + 1;   
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true, false);
  pBLEScan->setActiveScan(true); //we never get the  51 bytes long advertisment without this.
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(80);  // less or equal setInterval value
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}
