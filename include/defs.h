#ifndef DEFS_H
#define DEFS_H

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
#define SCREEN_ADDRESS 0X3C

#define SETUP_DELAY 100

// variables for drawing lines around the helm
uint8_t _target_point_radius = 10;
uint8_t _target_point_x_offset = (SCREEN_WIDTH/2);
uint8_t _y_offset = 15;
uint8_t _target_point_line_marker_height = 6;
uint8_t _center_circle_radius = 2;
uint8_t _rotation_factor = 3;
uint8_t _target_point_rotator_length = 6;
uint8_t _change_x_offset = 2; // how much to change x when the _update_helm_and_tiller function is called

float roll_angle, pitch_angle;
float acc_x, acc_y, acc_z;

float old_roll_angle = 0, new_roll_angle, change_in_roll_angle;

float conversion_factor = 3.142/180; // value to convert radians to degrees

float radius;
float x_roll, y_roll;
uint8_t _tiller_rectangle_offset = 10;
uint8_t _marking_triangle_width = 5;

#endif