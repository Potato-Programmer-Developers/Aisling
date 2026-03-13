/*
This file contains variable declarations and function prototypes for the
mc module.
*/

#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdbool.h>
#include "settings.h"

typedef struct Character{
    Texture2D sprite_idle;
    Texture2D sprite_walk;
    Texture2D sprite_run;
    Texture2D sprite;
    Vector2 position;
    Vector2 size;
    float speed;
    int direction;

    Rectangle frame_rect;
    int frame_number;
    int current_frame;
    int frame_counter;
    int frame_speed;
} Character;


Character InitCharacter(Settings* game_settings);
void UpdateCharacter(Character* character, Settings* game_settings);
void CloseCharacter(Character* character);
void DrawCharacter(Character* character);
#endif