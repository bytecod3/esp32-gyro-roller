#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "defs.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

void _display_settings(){
  display.setTextSize(1);
  display.setTextColor(WHITE);

}

void _draw_target_circle(){
  display.setCursor();
  display.drawCircle();
}

void setup(){               
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // loop forever
  }

  _display_settings();

  // Clear the buffer.
  display.clearDisplay();

  display.setCursor(0, 28);
  display.println("ESP32 GYRO");

  display.display();

}


void loop() {
  // put your main code here, to run repeatedly:
}