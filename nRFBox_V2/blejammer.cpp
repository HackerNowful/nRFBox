/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   Modified By
   https://github.com/HackerNowful/nRFBox-V2
   ________________________________________ */

#include <Arduino.h>
#include "blejammer.h"
#include "config.h"



#define MODE_BUTTON 33

RF24 radio1(CE1, CSN1, 16000000);
RF24 radio2(CE2, CSN2, 16000000);
RF24 radio3(CE3, CSN3, 16000000);

enum OperationMode { DEACTIVE_MODE, SINGLE_MODULE, MULTI_MODULE };
OperationMode currentMode = DEACTIVE_MODE;

const byte channels[] = {2, 26, 80};
byte currentChannelIndex = 0;

volatile bool modeChangeRequested = false;

unsigned long lastJammingTime = 0;
const unsigned long jammingInterval = 10;

unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 50;


void IRAM_ATTR handleButtonPress() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonPressTime > debounceDelay) {
    modeChangeRequested = true;
    lastButtonPressTime = currentTime;
  }
}

void configureRadio(RF24 &radio, byte channel) {
  radio.powerDown();
  delay(500);
  radio.powerUp();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.stopListening();
  radio.setChannel(channel);
}

void initializeRadiosMultiMode() {
  if (radio1.begin()) {
    Serial.println("Multi-Module Mode: Radio 1 started");
    configureRadio(radio1, 80);
  } else {
    Serial.println("Failed to initialize Radio 1");
  }

  if (radio2.begin()) {
    Serial.println("Multi-Module Mode: Radio 2 started");
    configureRadio(radio2, 26);
  } else {
    Serial.println("Failed to initialize Radio 2");
  }

  if (radio3.begin()) {
    Serial.println("Multi-Module Mode: Radio 3 started");
    configureRadio(radio3, 2);
  } else {
    Serial.println("Failed to initialize Radio 3");
  }

  Serial.println("All radios configured and ready in Multi-Module Mode");
}

void initializeRadios() {
  if (currentMode == SINGLE_MODULE) {
    if (radio1.begin()) {
      Serial.println("Single Module Mode: Radio 1 started");
      configureRadio(radio1, channels[currentChannelIndex]);
    } else {
      Serial.println("Failed to initialize Radio 1 in Single Module Mode");
    }
  } else if (currentMode == MULTI_MODULE) {
    initializeRadiosMultiMode();
  } else if (currentMode == DEACTIVE_MODE) {
    Serial.println("Deactive Mode: All radios are turned off");
    radio1.powerDown();
    radio2.powerDown();
    radio3.powerDown();
  }
}

void jammer(RF24 &radio, int channel) {
  const char text[] = "xxxxxxxxxxxxxxxx";
  for (int i = (channel * 5) + 1; i < (channel * 5) + 23; i++) {
    radio.setChannel(i);
    bool result = radio.write(&text, sizeof(text));
    if (result) {
      Serial.println("Transmission successful");
    } else {
      Serial.println("Transmission failed");
    }
    delay(10);
  }
}

void updateOLED() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print("Mode ");
  u8g2.print(" ........ ");
  u8g2.setCursor(70, 10);
  u8g2.print("[");
  u8g2.print(currentMode == SINGLE_MODULE ? "Single" : currentMode == MULTI_MODULE ? "Multi" : "Deactive");
  u8g2.print("]");

  u8g2.setCursor(0, 35);
  u8g2.print("Radio 1: ");
  u8g2.setCursor(70, 35);
  u8g2.print(radio1.isChipConnected() ? "Active" : "Inactive");

  u8g2.setCursor(0, 50);
  u8g2.print("Radio 2: ");
  u8g2.setCursor(70, 50);
  u8g2.print(radio2.isChipConnected() ? "Active" : "Inactive");

  u8g2.setCursor(0, 64);
  u8g2.print("Radio 3: ");
  u8g2.setCursor(70, 64);
  u8g2.print(radio3.isChipConnected() ? "Active" : "Inactive");

  u8g2.sendBuffer();
}

void checkModeChange() {
  if (modeChangeRequested) {
    modeChangeRequested = false;
    currentMode = static_cast<OperationMode>((currentMode + 1) % 3);
    Serial.println(currentMode == SINGLE_MODULE ? "Switched to Single Module Mode" :
                   currentMode == MULTI_MODULE ? "Switched to Multi-Module Mode" :
                   "Switched to Deactive Mode");

    initializeRadios();
    updateOLED();
  }
}


void blejammerSetup() {


  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();

  pinMode(MODE_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), handleButtonPress, FALLING);

  initializeRadios();
  updateOLED();

}

void blejammerLoop() {
  while (true) {
    checkModeChange();

    if (currentMode == SINGLE_MODULE) {
      if (millis() - lastJammingTime >= jammingInterval) {
        jammer(radio1, channels[currentChannelIndex]);
        currentChannelIndex = (currentChannelIndex + 1) % 3;
        Serial.print("Single Module Mode: Jamming on channel range ");
        Serial.println(channels[currentChannelIndex]);
        lastJammingTime = millis();
      }
    } else if (currentMode == MULTI_MODULE) {
      if (millis() - lastJammingTime >= jammingInterval) {
        jammer(radio1, 80);
        jammer(radio2, 26);
        jammer(radio3, 2);

        Serial.println("Multi-Module Mode: Jamming on all channel ranges");

        lastJammingTime = millis();
        updateOLED();
      }
    }
    if (digitalRead(BUTTON_BACK_PIN) == LOW) {
      // Exit bleSpooferloop and return to main menu
      current_screen = 0;
      drawMenu();
      break;
    }
  }
}
