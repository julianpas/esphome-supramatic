/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
const BLEAddress garagedooraddr("FB:89:5D:2C:2E:CD");


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.getAddress() == garagedooraddr) {       
        Serial.printf("Advertised Device: %s %d\n", advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getPayloadLength());
        uint8_t *payload = advertisedDevice.getPayload();       
        for (size_t i = 0; i < advertisedDevice.getPayloadLength();) {
          int field_len = payload[i];
          int field_type = payload[i+1];
          if (field_type == ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE) {
            char* pHex = BLEUtils::buildHexData(nullptr, payload + i + 2, field_len - 2);
            Serial.println(pHex);
          	free(pHex);
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
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(80);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  //delay(2000);
}
