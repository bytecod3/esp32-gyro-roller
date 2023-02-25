#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <ESPAsyncWebServer.h>
#include "defs.h"
#include "debug.h"
#include <wifi_config.h>

Adafruit_MPU6050 gyroscope;

// create async web server
AsyncWebServer server(80);

void _init_MPU6050(){
  /*
    Initialize MPU6050
  */

  // attempt to initialize MPU6050
  if(!gyroscope.begin(0x68)){
    debugln("MPU6050 allocation failed!");
    for(;;);
  }

  gyroscope.setAccelerometerRange(MPU6050_RANGE_8_G);
  gyroscope.setGyroRange(MPU6050_RANGE_500_DEG);
  gyroscope.setFilterBandwidth(MPU6050_BAND_5_HZ);

  delay(SETUP_DELAY);

}

void  _update_helm_and_tiller(){
  // if change is +ve, rotate the wheel clockwise
  // otherwise rotate the wheel anticlockwise
  // using polar coordinates

  // this rectangle willl fill the whole screen 
  display.fillRect(0,0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  // draw target outer circle
  display.drawCircle(_target_point_x_offset, _y_offset+_target_point_radius, _target_point_radius+_target_point_line_marker_height/2, WHITE);
  
  // lines around the target circle
  display.drawFastVLine(SCREEN_WIDTH/2, _target_point_radius-_target_point_line_marker_height, _target_point_line_marker_height, WHITE); // vertical top
  display.drawFastVLine(SCREEN_WIDTH/2, (_y_offset + _target_point_radius*2), _target_point_line_marker_height, WHITE); // vertical bottom
  display.drawFastHLine((SCREEN_WIDTH/2) - _target_point_radius-_target_point_line_marker_height, (_y_offset + _target_point_radius), _target_point_line_marker_height, WHITE ); // horizontal left
  display.drawFastHLine((SCREEN_WIDTH/2) + _target_point_radius, _y_offset + _target_point_radius, _target_point_line_marker_height, WHITE ); // horizontal right
  display.drawCircle(SCREEN_WIDTH/2, _y_offset + _target_point_radius, _center_circle_radius, WHITE); // center most circle

  // draw left y ticks
  uint8_t _y_tick_spacing = 10;
  uint8_t _y_tick_width = 5;

  for(uint8_t j = 10; j < SCREEN_HEIGHT; j += _y_tick_spacing){
    display.drawFastHLine(0, j, _y_tick_width, WHITE);
  }

  // draw right y ticks
  for(uint8_t j = 10; j < SCREEN_HEIGHT; j += _y_tick_spacing){
    display.drawFastHLine(SCREEN_WIDTH-_y_tick_width, j, _y_tick_width, WHITE);
  }

  display.display(); // enable displaying
}

void _establish_wifi_connection(){
  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    debugln("[+] Connecting to WiFi...");
    delay(200);
  }

  // show the IP address of server if connected to WIFI
  debugln("+ Connected with IP_ADR:");
  debug(WiFi.localIP()); debugln();

}

void setup(){               
  Serial.begin(115200);

  _init_MPU6050();
  _establish_wifi_connection();

  // send roll angle to client
  server.on("/get_roll", HTTP_GET, [](AsyncWebServerRequest *request){
    // debugln("[+]Request received. Processing...");
    String roll_angle_str;  
    roll_angle_str = roll_angle;
    request->send(200, "text/plain", roll_angle_str);
  });

  // start server
  server.begin();

}

void loop() {
  // get gyroscope readings
  sensors_event_t a, g, temp;
  gyroscope.getEvent(&a, &g, &temp);

  acc_x = a.acceleration.x;
  acc_y = a.acceleration.y;
  acc_z = a.acceleration.z;

  // measure and convert rotational angles
  roll_angle = (atan(acc_y/sqrt(acc_z*acc_z + acc_x*acc_x))) / conversion_factor;
  pitch_angle = (atan(-acc_x/sqrt(acc_y*acc_y + acc_z*acc_z))) / conversion_factor;

  // calculate the polar coordinates for the roll
  x_roll = cos(radians(roll_angle))*_marker_radius;
  y_roll = -sin(radians(roll_angle))*_marker_radius;

  x_roll_inverse = -cos(radians(roll_angle))*_marker_radius;
  y_roll_inverse = sin(radians(roll_angle))*_marker_radius;


  delay(100);

}