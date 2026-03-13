/*
This module stores the implementation of the variable and functions
contained in the mc.h header file.
*/

#include "settings.h"
#include "raymath.h"
#include "character.h"

Character InitCharacter(Settings* game_settings){
    Character new_character = {0};
    new_character.sprite_idle = LoadTexture("../assets/images/character/idle.png");
    new_character.sprite_walk = LoadTexture("../assets/images/character/walk.png");
    new_character.sprite_run = LoadTexture("../assets/images/character/run.png");
    new_character.sprite = new_character.sprite_idle;
    new_character.position = (Vector2){(float)game_settings->window_width / 2, (float)game_settings->window_height / 2};
    new_character.size = (Vector2){200.0f, 200.0f};
    new_character.speed = game_settings->mc_speed;
    new_character.direction = 0;

    new_character.frame_number = 12;
    new_character.frame_speed = 8;
    new_character.current_frame = 0;
    new_character.frame_counter = 0;

    float frame_width = (float)new_character.sprite.width / new_character.frame_number;
    float frame_height = (float)new_character.sprite.height / 4;

    new_character.frame_rect = (Rectangle){new_character.position.x, new_character.position.y, frame_width, frame_height};
    
    return new_character;
}

void UpdateCharacter(Character* character, Settings* game_settings){
    bool is_moving = (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN));
    bool is_running = ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && is_moving);

    int sprite_columns;
    if (is_running){
        character->sprite = character->sprite_run;
        character->speed = game_settings->mc_speed * 1.5f;
        character->frame_speed = 12;
        character->frame_number = 8;
        sprite_columns = 8;
    } else if (is_moving){
        character->sprite = character->sprite_walk;
        character->speed = game_settings->mc_speed;
        character->frame_speed = 8;
        character->frame_number = 6;
        sprite_columns = 6;
    } else{
        character->sprite = character->sprite_idle;
        character->speed = game_settings->mc_speed;
        character->frame_speed = (character->direction == 3) ? 4 : 8;
        character->frame_number = (character->direction == 3) ? 4 : 12;
        sprite_columns = 12; // The idle sprite sheet ALWAYS has 12 columns
    }

    character->frame_rect.width = (float)character->sprite.width / sprite_columns;

    /* Update movement flags based on user input. */
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
        character->position.x -= character->speed;
        character->direction = 1;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
        character->position.x += character->speed;
        character->direction = 2;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)){
        character->position.y -= character->speed;
        character->direction = 3;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)){
        character->position.y += character->speed;
        character->direction = 0;
    }

    /* Update the character position clamping. */
    character->position.x = Clamp(character->position.x, 0.0f, (float)game_settings->window_width - character->size.x);
    character->position.y = Clamp(character->position.y, 0.0f, (float)game_settings->window_height - character->size.y);

    character->frame_counter++;
    if (character->frame_counter >= (60 / character->frame_speed)){
        character->frame_counter = 0;
        character->current_frame++;
    }
    if (character->current_frame >= character->frame_number){
        character->current_frame = 0;
    }

    character->frame_rect.x = character->current_frame * character->frame_rect.width;
    character->frame_rect.y = character->direction * character->frame_rect.height;
}

void CloseCharacter(Character* character){
    /* Unload character textures to free VRAM. */
    UnloadTexture(character->sprite_idle);
    UnloadTexture(character->sprite_walk);
    UnloadTexture(character->sprite_run);
}

void DrawCharacter(Character* character){
    Rectangle dest_rect = {character->position.x, character->position.y, character->size.x, character->size.y};
    DrawTexturePro(character->sprite, character->frame_rect, dest_rect, (Vector2){0, 0}, 0.0f, WHITE);
}