/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   Modified By
   https://github.com/HackerNowful/nRFBox-V2
   ________________________________________ */

#include <Arduino.h>
#include "scanner.h"
#include "config.h"


int line;
char grey[] = " .:-=+*aRW";

char filled = 'F';
char drawDirection = 'R';
char slope = 'W';

byte sensorArray[129];

byte getRegister(byte r) {
  byte c;

  digitalWrite(CSN1, LOW);
  SPI.transfer(r & 0x1F);
  c = SPI.transfer(0);
  digitalWrite(CSN1, HIGH);

  return c;
}

void setRegister(byte r, byte v) {
  digitalWrite(CSN1, LOW);
  SPI.transfer((r & 0x1F) | 0x20);
  SPI.transfer(v);
  digitalWrite(CSN1, HIGH);
}

void powerUp(void) {
  setRegister(NRF24_CONFIG, getRegister(NRF24_CONFIG) | 0x02);
  delayMicroseconds(130);
}

void powerDown(void) {
  setRegister(NRF24_CONFIG, getRegister(NRF24_CONFIG) & ~0x02);
}

void enable(void) {
  digitalWrite(CE1, HIGH);
}

void disable(void) {
  digitalWrite(CE1, LOW);
}

void setRX(void) {
  setRegister(NRF24_CONFIG, getRegister(NRF24_CONFIG) | 0x01);
  enable();
  delayMicroseconds(100);
}

void scanChannels(void) {
  disable();
  for (int j = 0; j < 55; j++) {
    for (int i = 0; i < CHANNELS; i++) {
      setRegister(NRF24_RF_CH, (128 * i) / CHANNELS);
      setRX();
      delayMicroseconds(40);
      disable();
      if (getRegister(NRF24_RPD) > 0) channel[i]++;
    }
  }
}

void outputChannels(void) {
  int norm = 0;

  for (int i = 0; i < CHANNELS; i++)
    if (channel[i] > norm) norm = channel[i];

  Serial.print('|');
  for (int i = 0; i < CHANNELS; i++) {
    int pos;
    if (norm != 0) pos = (channel[i] * 10) / norm;
    else pos = 0;
    if (pos == 0 && channel[i] > 0) pos++;
    if (pos > 9) pos = 9;
    Serial.print(grey[pos]);
    channel[i] = 0;
  }

  Serial.print("| ");
  Serial.println(norm);

  u8g2.clearBuffer();

  u8g2.drawLine(0, 0, 0, 63);
  u8g2.drawLine(127, 0, 127, 63);

  for (byte count = 0; count < 64; count += 10) {
    u8g2.drawLine(127, count, 122, count);
    u8g2.drawLine(0, count, 5, count);
  }

  for (byte count = 10; count < 127; count += 10) {
    u8g2.drawPixel(count, 0);
    u8g2.drawPixel(count, 63);
  }

  if (norm < 10) {
    byte drawHeight = map(norm, 0, 20, 0, 64);
    sensorArray[0] = drawHeight;
  }
  if (norm > 10) {
    byte drawHeight = map(norm, 0, 40, 0, 64);
    sensorArray[0] = drawHeight;
  }




  for (byte count = 1; count <= 127; count++) {
    if (filled == 'D' || filled == 'd') {
      if (drawDirection == 'L' || drawDirection == 'l') {
        u8g2.drawPixel(count, 63 - sensorArray[count - 1]);
      } else {
        u8g2.drawPixel(127 - count, 63 - sensorArray[count - 1]);
      }
    } else {
      if (drawDirection == 'L' || drawDirection == 'l') {
        if (slope == 'W' || slope == 'w') {
          u8g2.drawLine(count, 63, count, 63 - sensorArray[count - 1]);
        } else {
          u8g2.drawLine(count, 0, count, 63 - sensorArray[count - 1]);
        }
      } else {
        if (slope == 'W' || slope == 'w') {
          u8g2.drawLine(127 - count, 63, 127 - count, 63 - sensorArray[count - 1]);
        } else {
          u8g2.drawLine(127 - count, 0, 127 - count, 63 - sensorArray[count - 1]);
        }
      }
    }
  }

  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(12, 12);
  u8g2.print(norm);

  u8g2.sendBuffer();

  for (byte count = 127; count >= 2; count--) {
    sensorArray[count - 1] = sensorArray[count - 2];
  }
}

void checkButtons() {
  static unsigned long lastDebounceTime1 = 0;
  static int lastButtonState1 = LOW;

  int buttonState1 = digitalRead(BUTTON_UP_PIN);

  if (buttonState1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }
  if ((millis() - lastDebounceTime1) > 50) {
    if (buttonState1 == LOW) {
      if (filled == 'F') filled = 'D';
      else filled = 'F';
      Serial.println(filled);
    }
  }
  lastButtonState1 = buttonState1;
}


void scannerSetup() {
  Serial.begin(115200);

  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();

  u8g2.begin();

  for (byte count = 0; count <= 128; count++) {
    sensorArray[count] = 0;
  }

  Serial.println("Starting 2.4GHz Scanner ...");
  Serial.println();

  SPI.begin(18, 19, 23, 17);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(10000000);
  SPI.setBitOrder(MSBFIRST);

  pinMode(CE1, OUTPUT);
  pinMode(CSN1, OUTPUT);

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);

  disable();

  powerUp();
  setRegister(NRF24_EN_AA, 0x0);
  setRegister(NRF24_RF_SETUP, 0x0F);
}

void scannerLoop() {
  while (true) {
    checkButtons();
    scanChannels();
    outputChannels();
    if (digitalRead(BUTTON_BACK_PIN) == LOW) {
      break;
    }
  }

}
