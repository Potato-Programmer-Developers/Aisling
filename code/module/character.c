/**
 * @file character.c
 * @brief Implementation of player character systems.
 * 
 * Manages player input, physics (movement & collision), animation states, 
 * and gameplay mechanics like stamina and hallucination.
 * 
 * Authors: Andrew Zhuo
 */

#include "character.h"
#include "raylib.h"
#include "raymath.h"
#include "settings.h"
#include "map.h"
#include "data.h"

/**
 * @brief Initializes the main character with default values or saved data.
 * 
 * @param game_settings Gameplay constants (speed, max stamina).
 * @param game_data Optional saved data to restore position/inventory (unused here but hook exists).
 * @param game_map Pointer to map for spawn positioning logic.
 * @return Initialized Character structure.
 */
Character InitCharacter(Settings *game_settings, Data *game_data, Map *game_map){
	Character new_character = {0};

	// 1. Load Animation Textures
	new_character.sprite_idle = LoadTexture("../assets/images/character/idle.png");
	new_character.sprite_walk = LoadTexture("../assets/images/character/walk.png");
	new_character.sprite_run = LoadTexture("../assets/images/character/run.png");
	new_character.sprite = new_character.sprite_idle;

	// 2. Set Physics Constants
	new_character.size = (Vector2){400.0f, 400.0f};
	new_character.position = (Vector2){ 450.0f, 1200.0f }; // HARDCODED spawn for Stage 1
	new_character.speed = game_settings->mc_speed;
	new_character.direction = 0; // 0: Down, 1: Left, 2: Right, 3: Up

	// 3. Initialize Animation Logic
	new_character.frame_number = 12;
	new_character.frame_speed = 8;
	new_character.current_frame = 0;
	new_character.frame_counter = 0;

	// 4. Set Gameplay Mechanics
	new_character.max_stamina = game_settings->max_stamina;
	new_character.stamina = new_character.max_stamina;
	new_character.exhausted = false;
	new_character.needs_shift_reset = false;

	new_character.max_hallucination = 100.0f;
	new_character.hallucination = 0.0f;

	// 5. Calculate Sprite Source Rectangle
	float frame_width = (float)new_character.sprite.width / new_character.frame_number;
	float frame_height = (float)new_character.sprite.height / 4;   // 4 direction rows
	new_character.frame_rect = (Rectangle){0.0f, 0.0f, frame_width, frame_height};

	return new_character;
}

/**
 * @brief Main update logic for the character.
 * 
 * Processes input, calculates movement vectors, handles collision detection,
 * manages the stamina/exhaustion cycle, and updates the animation state.
 */
void UpdateCharacter(Character *character, Settings *game_settings, Vector2 map_size, Map *map, Audio* audio, bool is_outdoor){
    Vector2 movement = {0, 0};
    
    // --- Phase 1: Input Processing ---
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) movement.x -= 1;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) movement.x += 1;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) movement.y -= 1;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) movement.y += 1;

    bool is_moving = (movement.x != 0 || movement.y != 0);

    // Update facing direction based on movement
    if (is_moving) {
        if (movement.x < 0) character->direction = 1;      // Left
        else if (movement.x > 0) character->direction = 2; // Right
        else if (movement.y < 0) character->direction = 3; // Up
        else if (movement.y > 0) character->direction = 0; // Down
    }

    // --- Phase 2: Stamina and Run Logic ---
    if (character->needs_shift_reset){
        // Ensure player releases Shift after exhaustion before they can run again
        if (!character->exhausted && ((!IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT)) || !is_moving)){
            character->needs_shift_reset = false;
        }
    }

    bool is_running = ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && 
	                  is_moving && character->stamina > 0 && 
	                  !character->exhausted && !character->needs_shift_reset);

    float frame_time = GetFrameTime();  
	
    if (is_running){
        // Hallucination Penalty: increase stamina drain as hallucination grows above 75%
        float depletion_multiplier = 1.0f;
        if (character->hallucination > character->max_hallucination * 0.75f){
            depletion_multiplier += (character->hallucination - character->max_hallucination * 0.75f) / (character->max_hallucination * 0.1f);
        }
        
        character->stamina -= game_settings->stamina_depletion_rate * depletion_multiplier * frame_time;
        if (character->stamina <= 0){
            character->stamina = 0;
            character->exhausted = true;
            character->needs_shift_reset = true; 
        }
    } else {
        // Natural stamina recovery when not running
        character->stamina += game_settings->stamina_recovery_rate * frame_time;
        if (character->stamina > character->max_stamina) character->stamina = character->max_stamina;
        
        // Recover from exhausted state at 10% threshold
        if (character->exhausted && character->stamina >= character->max_stamina * 0.1f) character->exhausted = false;
    }

    // --- Phase 3: Hallucination growth (Outdoor Only) ---
    if (is_outdoor){
        character->hallucination += game_settings->hallucination_increase_rate * frame_time;
        // Cap at 200% for heavy darkening effects
        if (character->hallucination > character->max_hallucination * 2.0f) character->hallucination = character->max_hallucination * 2.0f;
    }

    // --- Phase 4: Animation and Speed State Management ---
    int sprite_columns;
    if (is_running){
        character->sprite = character->sprite_run;
        character->speed = game_settings->mc_speed * 2.0f;
        character->frame_speed = 12;
        character->frame_number = 8;
        sprite_columns = 8;
    } else if (is_moving){
        character->sprite = character->sprite_walk;
        character->speed = game_settings->mc_speed;
        character->frame_speed = 8;
        character->frame_number = 6;
        sprite_columns = 6;
    } else {
        character->sprite = character->sprite_idle;
        character->speed = game_settings->mc_speed;
        character->frame_speed = (character->direction == 3) ? 4 : 8; // Slower idle for looking up
        character->frame_number = (character->direction == 3) ? 4 : 12;
        sprite_columns = 12;
    }

    character->frame_rect.width = (float)character->sprite.width / sprite_columns;

    // --- Phase 5: Physics (Movement & Collision) ---
    if (is_moving) {
        movement = Vector2Normalize(movement);
        movement.x *= character->speed * frame_time;
        movement.y *= character->speed * frame_time;
    }

    // Collision Check: Horizontal
    float next_x = character->position.x + movement.x;
    next_x = Clamp(next_x, 0.0f, map_size.x - character->size.x);
    Rectangle collision_rect_x = {next_x + 75, character->position.y + 100, character->size.x - 150, character->size.y - 150};
    if (!CheckMapCollision(map, collision_rect_x)) character->position.x = next_x;

    // Collision Check: Vertical
    float next_y = character->position.y + movement.y;
    next_y = Clamp(next_y, 0.0f, map_size.y - character->size.y);
    Rectangle collision_rect_y = {character->position.x + 75, next_y + 100, character->size.x - 150, character->size.y - 150};
    if (!CheckMapCollision(map, collision_rect_y)) character->position.y = next_y;

    // --- Phase 6: Frame Advancement & Audio Triggering ---
    character->frame_counter++;
    if (character->frame_counter >= (60 / character->frame_speed)){
        character->frame_counter = 0;
        character->current_frame++;
        if (character->current_frame >= character->frame_number) character->current_frame = 0;

        // Audio: Trigger footstep sounds on specific frames
        if (is_running){
            if (character->current_frame == 0 || character->current_frame == 4) PlayStep(audio, is_outdoor);
        } else if (is_moving){
            if (character->current_frame == 0 || character->current_frame == 3) PlayStep(audio, is_outdoor);
        }
  
        // Sync the sprite source rectangle to current frame/direction
        character->frame_rect.x = character->current_frame * character->frame_rect.width;
        character->frame_rect.y = character->direction * character->frame_rect.height;
    }
}

/**
 * @brief Frees GPU memory holding player textures.
 */
void CloseCharacter(Character *character){
    UnloadTexture(character->sprite_idle);
    UnloadTexture(character->sprite_walk);
    UnloadTexture(character->sprite_run);
}

/**
 * @brief Renders the character and their state indicators (stamina, hallucination).
 */
void DrawCharacter(Character *character){
    // 1. Draw Character Sprite
    Rectangle dest_rect = {character->position.x, character->position.y, character->size.x, character->size.y};
    DrawTexturePro(character->sprite, character->frame_rect, dest_rect, (Vector2){0, 0}, 0.0f, WHITE);

    // 2. Visual Utility: Stamina Bar (right side, vertical)
    if (character->stamina < character->max_stamina){
        float bar_width = 10.0f;
        float bar_height = character->size.y * 0.6f;
        float posX = character->position.x + character->size.x + 5.0f;
        float posY = character->position.y + (character->size.y - bar_height) / 2.0f;
        
        DrawRectangleRec((Rectangle){posX, posY, bar_width, bar_height}, Fade(BROWN, 0.5f));
        DrawRectangleLinesEx((Rectangle){posX - 1, posY - 1, bar_width + 2, bar_height + 2}, 1, BLACK);
        
        float current_bar_height = character->stamina / character->max_stamina * bar_height;
        DrawRectangleRec((Rectangle){posX, posY + (bar_height - current_bar_height), bar_width, current_bar_height}, ORANGE);
    }

    // 3. Visual Utility: Hallucination Bar (left side, vertical)
    if (character->hallucination > 0){
        float bar_width = 10.0f;
        float bar_height = character->size.y * 0.6f;
        float stageX = character->position.x - bar_width - 5.0f;
        float stageY = character->position.y + (character->size.y - bar_height) / 2.0f;
        float h_percent = character->hallucination / character->max_hallucination;
        if (h_percent > 1.0f) h_percent = 1.0f;
        float current_bar_height = h_percent * bar_height;

        DrawRectangleRec((Rectangle){stageX, stageY, bar_width, bar_height}, Fade(GRAY, 0.5f));
        DrawRectangleLinesEx((Rectangle){stageX - 1, stageY - 1, bar_width + 2, bar_height + 2}, 1, BLACK);
        DrawRectangleRec((Rectangle){stageX, stageY + (bar_height - current_bar_height), bar_width, current_bar_height}, PURPLE);
    }
}