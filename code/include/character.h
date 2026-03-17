/*
This file contains variable declarations and function prototypes for the
mc module.

Module made by Andrew Zhuo.
*/

#ifndef CHARACTER_H
#define CHARACTER_H

#define MAX_INVENTORY_SIZE 6
#define MAX_ITEM_NAME_LENGTH 10

#include <stdbool.h>
#include "settings.h"

typedef struct Map Map;
typedef struct Data Data;

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
    Rectangle bounds;
    int frame_number;
    int current_frame;
    int frame_counter;
    int frame_speed;

    char inventory[MAX_INVENTORY_SIZE][MAX_ITEM_NAME_LENGTH];
    int inventory_count;
} Character;


Character InitCharacter(Settings* game_settings, Data* game_data);                                                      // Initialize the character.
void UpdateCharacter(Character* character, Settings* game_settings, Vector2 map_size, Map* map);       // Update the character.
void CloseCharacter(Character* character);                                                             // Close the character.
void DrawCharacter(Character* character);                                                              // Draw the character.

#endif