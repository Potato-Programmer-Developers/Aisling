/*
This file contains the data structures for the game.

Made by Andrew Zhuo.
*/

#ifndef DATA_H
#define DATA_H

#include "raylib.h"
#include "character.h"
#include "settings.h"

typedef struct Data{
    Vector2 position;
    int direction;
    char inventory[MAX_INVENTORY_SIZE][MAX_ITEM_NAME_LENGTH];
    int inventory_count;

    int volume;
} Data;

Data LoadData(Settings* game_settings);
void ApplyData(Character* player, Settings* game_settings, Data* data);
void SaveData(Character* player, Settings* game_settings);

#endif