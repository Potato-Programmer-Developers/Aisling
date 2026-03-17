/*
This file contains function prototypes for the interaction module.

Module made by Cornelius Jabez Lim.
*/

#ifndef INTERACTION_H
#define INTERACTION_H

#include <stdbool.h>
#include "settings.h"
#include "raylib.h"

typedef struct {
    Rectangle bounds;
    const char* dialoguePath;
    bool isActive;
} Interactable;

#endif