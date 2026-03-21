/**
 * @file interaction.c
 * @brief Implementation of world interaction logic (NPCs, Items).
 * 
 * Handles loading world objects, checking proximity-based collisions,
 * and dispatching interaction events (dialogue or inventory pickup).
 * 
 * Authors: Andrew Zhuo and Cornelius Jabez Lim
 */

#include "interaction.h"
#include <string.h>
#include "raymath.h"

/** @brief Loads textures for an array of NPCs using their pre-defined paths. */
void LoadNPCs(NPC npcs[], int count){
    for (int i = 0; i < count; i++){
        npcs[i].base.texture = LoadTexture(npcs[i].base.texturePath);
    }
}

/** @brief Loads textures for an array of Items using their pre-defined paths. */
void LoadItems(Item items[], int count){
    for (int i = 0; i < count; i++){
        items[i].base.texture = LoadTexture(items[i].base.texturePath);
    }
}

/**
 * @brief Scans for the nearest interactable object within collision range.
 * 
 * Uses distance-to-center calculation after detection to ensure the player
 * interacts with the most relevant object when multiple are overlapping.
 */
void CheckInteractable(NPC worldNPCs[], Item worldItems[], int npcCount, int itemCount,
    Rectangle playerHitbox, Interactable** objectToInteractWith){
    
    *objectToInteractWith = NULL;
    float min_dist = 1e6; // Sentinel
    Vector2 playerPos = {playerHitbox.x + playerHitbox.width / 2.0f, playerHitbox.y + playerHitbox.height / 2.0f};

    // 1. Check Collision with NPCs
    for (int i = 0; i < npcCount; i++){
        if (CheckCollisionRecs(playerHitbox, worldNPCs[i].base.bounds)){
            worldNPCs[i].base.isActive = true; // Signals we can interact
            Vector2 npcPos = {worldNPCs[i].base.bounds.x + worldNPCs[i].base.bounds.width / 2.0f, 
                               worldNPCs[i].base.bounds.y + worldNPCs[i].base.bounds.height / 2.0f};
            float dist = Vector2Distance(playerPos, npcPos);
            if (dist < min_dist){
                min_dist = dist;
                *objectToInteractWith = (Interactable*)&worldNPCs[i];
            }
        } else {
            worldNPCs[i].base.isActive = false;
        }
    }

    // 2. Check Collision with Items
    for (int i = 0; i < itemCount; i++){
        // Exclude items already in pockets
        if (!worldItems[i].picked_up && CheckCollisionRecs(playerHitbox, worldItems[i].base.bounds)){
            worldItems[i].base.isActive = true;
            Vector2 itemPos = {worldItems[i].base.bounds.x + worldItems[i].base.bounds.width / 2.0f, 
                                worldItems[i].base.bounds.y + worldItems[i].base.bounds.height / 2.0f};
            float dist = Vector2Distance(playerPos, itemPos);
            if (dist < min_dist){
                min_dist = dist;
                *objectToInteractWith = (Interactable*)&worldItems[i];
            }
        } else {
            worldItems[i].base.isActive = false;
        }
    }
}

/**
 * @brief High-level dispatcher for the 'Interact' command (e.g. Spacebar).
 * 
 * If already in a cutscene, advances the dialogue.
 * Otherwise, triggers type-specific interaction for the target object.
 */
void InteractWithObject(Interactable* objectToInteractWith, Dialogue* game_dialogue, 
    GameState* game_state, Character *player){
    
    // Advancement Logic: if we are talking, we just move to next line
    if (*game_state == DIALOGUE_CUTSCENE) {
        InteractWithNPC(NULL, game_dialogue, game_state);
        return;
    }

    if (objectToInteractWith == NULL) return;

    // Type Dispatch
    switch (objectToInteractWith->type){
        case INTERACTABLE_TYPE_NPC:
            InteractWithNPC((NPC*)objectToInteractWith, game_dialogue, game_state);
            break;
        case INTERACTABLE_TYPE_ITEM:
            InteractWithItem((Item*)objectToInteractWith, player);
            break;
    }
}

/**
 * @brief Handles dialogue logic: loading scripts and advancing lines.
 */
void InteractWithNPC(NPC *npc, Dialogue *game_dialogue, GameState *game_state){
    if (*game_state == GAMEPLAY && npc != NULL){
        // Phase A: Init Conversation
        *game_dialogue = LoadDialogue(npc->dialoguePath);

        // Enter state only if script is non-empty
        if (game_dialogue->line_count > 0){
            *game_state = DIALOGUE_CUTSCENE;
        }
    } else if (*game_state == DIALOGUE_CUTSCENE){
        // Phase B: Advance Line
        game_dialogue->current_line++;

        // Phase C: Exit condition
        if (game_dialogue->current_line >= game_dialogue->line_count){
            *game_state = GAMEPLAY;
        }
    }
}

/**
 * @brief Transfers an item from the game world to the player's inventory.
 */
void InteractWithItem(Item *item, Character *player){
    // Stage 1 Simplification: use texture path as ID
    strcpy(player->inventory[player->inventory_count], item->base.texturePath);
    player->item_count[player->inventory_count]++;
    player->inventory_count++;
    
    // Remove from the world renderer
    item->picked_up = true;
}