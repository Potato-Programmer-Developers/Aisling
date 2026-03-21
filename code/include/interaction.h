/**
 * @file interaction.h
 * @brief Systems for player interaction with NPCs and items.
 * 
 * This module defines the base interactable structure and its derivatives
 * (NPCs, Items), along with logic for collision detection and triggering interactions.
 * 
 * Authors: Andrew Zhuo and Cornelius Jabez Lim
 */

#ifndef INTERACTION_H
#define INTERACTION_H

#include <stdbool.h>
#include "dialogue.h"
#include "raylib.h"
#include "state.h"
#include "character.h"

/**
 * @brief Categorizes the type of interactable object.
 */
typedef enum {
    INTERACTABLE_TYPE_NPC,   // Non-player characters with dialogue
    INTERACTABLE_TYPE_ITEM,  // Collectible world items
} InteractableType;

/**
 * @brief Base structure for all objects the player can interact with.
 */
typedef struct {
    Texture2D texture;             // Visual representation of the object
    const char* texturePath;       // Path to the texture file
    Rectangle bounds;              // World boundaries for interaction checks
    bool isActive;                 // Whether the object is currently visible/active
    InteractableType type;         // Discriminator for NPC vs Item logic
} Interactable;

/**
 * @brief Specialized interactable for talking to characters.
 */
typedef struct {
    Interactable base;             // Base interactable properties
    const char* dialoguePath;      // Path to the text file containing NPC dialogue
} NPC;

/**
 * @brief Specialized interactable for picking up items.
 */
typedef struct Item {
    Interactable base;             // Base interactable properties
    bool picked_up;                // Persistence flag: true if inventory has absorbed this item
} Item;

/**
 * @brief Loads visual assets for a collection of NPCs.
 * @param npcs Array of NPC structures.
 * @param count Number of NPCs in the array.
 */
void LoadNPCs(NPC npcs[], int count);

/**
 * @brief Loads visual assets for a collection of world items.
 * @param items Array of Item structures.
 * @param count Number of items in the array.
 */
void LoadItems(Item items[], int count);

/**
 * @brief Detects if the player is within interaction range of any world object.
 * @param worldNPCs Array of NPCs to check.
 * @param worldItems Array of Items to check.
 * @param npcCount Number of NPCs.
 * @param itemCount Number of items.
 * @param playerHitbox Rectangle representing the player's interaction reach.
 * @param objectToInteractWith Pointer to update with the identified target.
 */
void CheckInteractable(
    NPC worldNPCs[], Item worldItems[], int npcCount, int itemCount,
    Rectangle playerHitbox, Interactable** objectToInteractWith
);

/**
 * @brief High-level dispatcher for triggering an interaction.
 * @param objectToInteractWith The target object identified by the check function.
 * @param game_dialogue Pointer to the global dialogue state.
 * @param game_state Pointer to the current game state (to switch to dialogue mode).
 * @param player Pointer to character (for inventory updates).
 */
void InteractWithObject(
    Interactable* objectToInteractWith, Dialogue* game_dialogue,
    GameState* game_state, Character *player
);

/**
 * @brief Internal handler for NPC interaction (starts dialogue).
 */
void InteractWithNPC(NPC *npc, Dialogue *game_dialogue, GameState *game_state);

/**
 * @brief Internal handler for Item interaction (adds to inventory).
 */
void InteractWithItem(Item *item, Character *player);

#endif