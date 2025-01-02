#pragma once

/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   Modified By
   https://github.com/HackerNowful/nRFBox-V2
   ________________________________________ */
#include <vector>
#include "config.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);


void setupNeoPixel() {
  strip.begin();
  strip.clear();
  strip.show();
}


void setNeoPixelColour(String colour) {
  if (colour == "red") {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();
  } else if (colour == "green") {
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.show();
  } else if (colour == "blue") {
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.show();
  } else if (colour == "yellow") {
    strip.setPixelColor(0, strip.Color(255, 255, 0));
    strip.show();
  } else if (colour == "purple") {
    strip.setPixelColor(0, strip.Color(255, 0, 255));
    strip.show();
  } else if (colour == "cyan") {
    strip.setPixelColor(0, strip.Color(0, 255, 255));
    strip.show();
  } else if (colour == "white") {
    strip.setPixelColor(0, strip.Color(255, 255, 255));
    strip.show();
  } else if (colour == "off") {
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
  } else {
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
  }
}

void flash(int numberOfFlashes, const std::vector<String>& colors, String finalColour) {
  for (int i = 0; i < numberOfFlashes; ++i) {
    for (const auto& color : colors) {
      setNeoPixelColour(color);
      delay(500);
    }
  }
  setNeoPixelColour(finalColour);
}
