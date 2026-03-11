/*
This file contains variable declarations and function prototypes
for the settings module.
*/
#include "raylib.h"

#ifndef SETTINGS_H
#define SETTINGS_H

// Global variables for settings.
extern int window_width;
extern int window_height;
extern int fps;
extern int volume;
extern float mc_speed;
extern Vector2 mc_size;

int change_volume(int volume);

#endif