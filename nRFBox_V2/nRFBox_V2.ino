/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   Modified By
   https://github.com/HackerNowful/nRFBox-V2
   ________________________________________ */

#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "icon.h"
#include "scanner.h"
#include "analyzer.h"
#include "jammer.h"
#include "blejammer.h"
#include "spoofer.h"
#include "sourapple.h"
#include "config.h"
#include "neopixel.h"

#if defined(HACKCAT)
SPIClass spiHSPI(HSPI);
SPIClass spiVSPI(VSPI);
#endif

RF24 RadioA(CE1, CSN1);
RF24 RadioB(CE2, CSN2);
RF24 RadioC(CE3, CSN3);



const unsigned char* bitmap_icons[8] = {
  bitmap_icon_scanner,
  bitmap_icon_analyzer,
  bitmap_icon_jammer,
  bitmap_icon_ble_jammer,
  bitmap_icon_spoofer,
  bitmap_icon_apple,
  bitmap_icon_about
};




char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {
  { "Scanner" },
  { "Analyzer" },
  { "Jammer" },
  { "BLE Jammer" },
  { "BLE Spoofer" },
  { "Sour Apple" },
  { "About" }
};





void about() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(7, 15, "CiferTech@gmail.com");
  u8g2.drawStr(12, 35, "GitHub/cifertech");
  u8g2.drawStr(7, 55, "instagram/cifertech");
  u8g2.sendBuffer();
}

void configureNrf(RF24 &radio) {
  radio.begin();
  radio.powerDown();
  delay(500);
  radio.powerUp();
  //radio.setAutoAck(false);
  //radio.setPALevel(RF24_PA_HIGH);
  //radio.setDataRate(RF24_2MBPS);
  //radio.stopListening();
}

void drawMenu() {

  u8g2.clearBuffer();

  if (current_screen == 0) {

    u8g2.drawXBMP(0, 22, 128, 21, bitmap_item_sel_outline);

    u8g2.setFont(u8g_font_7x14);
    u8g2.drawStr(25, 15, menu_items[item_sel_previous]);
    u8g2.drawXBMP( 4, 2, 16, 16, bitmap_icons[item_sel_previous]);

    u8g2.setFont(u8g_font_7x14B);
    u8g2.drawStr(25, 15 + 20 + 2, menu_items[item_selected]);
    u8g2.drawXBMP( 4, 24, 16, 16, bitmap_icons[item_selected]);

    u8g2.setFont(u8g_font_7x14);
    u8g2.drawStr(25, 15 + 20 + 20 + 2 + 2, menu_items[item_sel_next]);
    u8g2.drawXBMP( 4, 46, 16, 16, bitmap_icons[item_sel_next]);

    u8g2.drawXBMP(128 - 8, 0, 8, 64, bitmap_scrollbar_background);

    u8g2.drawBox(125, 64 / NUM_ITEMS * item_selected, 3, 64 / NUM_ITEMS);
  }

  u8g2.sendBuffer();
}

void setup() {

#if defined(HACKCAT)
  spiHSPI.begin(14, 12, 13, -1); // HSPI: SCK=14, MISO=12, MOSI=13
  spiVSPI.begin(18, 19, 23, -1); // VSPI: SCK=18, MISO=19, MOSI=23
#endif
  configureNrf(RadioA);
  configureNrf(RadioB);
  configureNrf(RadioC);

  u8g2.begin();
  u8g2.setBitmapMode(1);
  setupNeoPixel();

  u8g2.clearBuffer();

  //u8g2.drawXBMP(0, 0, 128, 64, logo_cifer);
#if defined(nRFBox_V2)
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(15, 35);
  u8g2.print("nRF-BOX");

  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(15, 50);
  u8g2.print("by CiferTech");

  u8g2.sendBuffer();
  delay(3000);
#elif defined(HACKCAT)

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(15, 35);
  u8g2.print("nRF-BOX");

  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(15, 50);
  u8g2.print("on HACKCAT");

  u8g2.sendBuffer();
  delay(1500);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(15, 15);
  u8g2.print("By -");
  u8g2.setCursor(15, 28);
  u8g2.print("CiferTech");
  u8g2.drawStr(15, 43, "Modified by");
  u8g2.drawStr(15, 58, "Hacker Nowful");
  u8g2.sendBuffer();
  delay(1500);
#endif

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  drawMenu();

}


void loop() {

  if (current_screen == 0) { // MENU SCREEN

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0)) {
      item_selected = item_selected - 1;
      button_up_clicked = 1;
      if (item_selected < 0) {
        item_selected = NUM_ITEMS - 1;
      }
    }
    else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0)) {
      item_selected = item_selected + 1;
      button_down_clicked = 1;
      if (item_selected >= NUM_ITEMS) {
        item_selected = 0;
      }
    }

    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1)) {
      button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1)) {
      button_down_clicked = 0;
    }
  }


  bool callAbout = true;

  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) {
    button_select_clicked = 1;


    if (current_screen == 0 && item_selected == 6) {
      while (item_selected == 6) {
        if (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
          if (callAbout) {
            about();
            callAbout = false;  // Toggle the state to not call about()
          } else {
            break;  // Toggle the state to break the loop
            callAbout = true;  // Reset the state for the next cycle
          }

          while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
            // Wait for the button to be released
            if (callAbout = true) {
              break;
            }
          }
        }
      }
    }
    else if (current_screen == 0 && item_selected == 5) {
      sourappleSetup();
      delay(500);
      sourappleLoop();
    }
    else if (current_screen == 0 && item_selected == 4) {
      bleSpooferSetup();
      delay(500);
      bleSpooferloop();
    }
    else if (current_screen == 0 && item_selected == 3) {
      blejammerSetup();
      delay(500);
      blejammerLoop();

    }
    else if (current_screen == 0 && item_selected == 2) {
      jammerSetup();
      delay(500);
      jammerLoop();
    }
    else if (current_screen == 0 && item_selected == 1) {
      analyzerSetup();
      delay(500);
      analyzerLoop();
    }
    else if (current_screen == 0 && item_selected == 0) {
      scannerSetup();
      delay(500);
      scannerLoop();
    }
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) {
    button_select_clicked = 0;
  }


  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {
    item_sel_previous = NUM_ITEMS - 1;
  }
  item_sel_next = item_selected + 1;
  if (item_sel_next >= NUM_ITEMS) {
    item_sel_next = 0;
  }
  drawMenu();
}
