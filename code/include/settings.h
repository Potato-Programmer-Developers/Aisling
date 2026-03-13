/*
This file contains variable declarations and function prototypes for the
settings module.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "raylib.h"


typedef struct Settings{
    int window_width;
    int window_height;
    int fps;
    float game_volume;
    float mc_speed;
} Settings;

Settings InitSettings();
void change_volume(Settings* game_settings);

#endif