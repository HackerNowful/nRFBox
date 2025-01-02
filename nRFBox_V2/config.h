#pragma once

/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   Modified By
   https://github.com/HackerNowful/nRFBox-V2
   ________________________________________ */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>
#include "esp_bt.h"
#include "esp_wifi.h"

#define HACKCAT
//#define nRFBox_V2

#if defined(HACKCAT)

#define BUTTON_UP_PIN 34
#define BUTTON_SELECT_PIN 33
#define BUTTON_DOWN_PIN 32
#define BUTTON_BACK_PIN 35

#define deviceTypeNextPin 32
#define deviceTypePrevPin 34
#define advControlPin 33

#define SDA 21
#define SCL 22

#define LED_PIN  13

#define CE2 2
#define CSN2 17

#define CE1  26
#define CSN1 25

#define CE3  16
#define CSN3 15

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

#elif defined(nRFBox_V2)

#define BUTTON_UP_PIN 26
#define BUTTON_SELECT_PIN 32
#define BUTTON_DOWN_PIN 33
#define BUTTON_BACK_PIN 25

const int deviceTypeNextPin = 26; 
const int deviceTypePrevPin = 33; 
const int advTypeNextPin = 25;    
//const int advTypePrevPin = 27;    
const int advControlPin = 27;     

#define CE1  5
#define CSN1 17

#define CE2  16
#define CSN2 4

#define CE3  15
#define CSN3 2

#define LED_PIN 14
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0); // [full framebuffer, size = 1024 bytes]
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#endif


int button_up_clicked = 0;
int button_select_clicked = 0;
int button_down_clicked = 0;

int item_selected = 0;

int item_sel_previous;
int item_sel_next;

int current_screen = 0;
const int NUM_ITEMS = 7;
const int MAX_ITEM_LENGTH = 20;

#define NRF24_CONFIG      0x00
#define NRF24_EN_AA       0x01
#define NRF24_RF_CH       0x05
#define NRF24_RF_SETUP    0x06
#define NRF24_RPD         0x09

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define CHANNELS  64
int channel[CHANNELS];
void drawMenu();
