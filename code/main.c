/*
This file contains the main logic for the game.

Made by Andrew Zhuo, Cornelius Jabez Lim, and Steven Kenneth Darwy
*/

#include "audio.h"
#include "character.h"
#include "dialogue.h"
#include "game_context.h"
#include "interaction.h"
#include "map.h"
#include "raylib.h"
#include "scene.h"
#include "settings.h"
#include "state.h"
#include <stdio.h>
#include "data.h"

void InitGame(Settings *game_settings);
void RunGame(Character *player, Audio *game_audio, Settings *game_settings,
             Scene *game_scene, Interactive *game_interactive,
             Dialogue *intro_dialogue, Map *game_map,
             Interactable *worldObjects, GameContext *game_context,
             GameState *game_state);
void EndGame(Audio *game_audio, Character *player, Scene *game_scene,
             Interactive *game_interactive, Map *game_map, Settings *game_settings);

int main(void) {
  /* Initialize the game */

  // Initialize the settings and game.
  Settings game_settings = InitSettings();
  InitGame(&game_settings);

  // Load game data.
  Data game_data = LoadData(&game_settings);
  
  // Load game resources.
  Character player = InitCharacter(&game_settings, &game_data);
  Audio game_audio = InitAudio(&game_settings);
  Scene game_scene = InitScene(&game_settings);
  Interactive game_interactive = InitInteractive(&game_settings);
  Dialogue game_dialogue = LoadDialogue("../assets/text/dialogue1.txt");
  Map game_map = InitMap("../assets/map/map.json");
  Interactable worldObjects[2] = {
      {{150, 200, 50, 50}, "../assets/text/signpost.txt", false},
      {{600, 300, 60, 60}, "../assets/text/oldman.txt", false}};
  GameContext game_context = InitGameContext(&game_map, &player, &game_settings);
  GameState game_state = MAINMENU;
  ApplyData(&player, &game_settings, &game_data);
  
  // Run the game.
  RunGame(&player, &game_audio, &game_settings, &game_scene, &game_interactive,
    &game_dialogue, &game_map, worldObjects, &game_context, &game_state);
    
    // End the game.
    EndGame(&game_audio, &player, &game_scene, &game_interactive, &game_map, &game_settings);
    
    return 0;
}

void InitGame(Settings *game_settings){
    /* Initialize the game */
    
    // Prepare and initialize the game windows.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(game_settings->fps);
    InitWindow(game_settings->window_width, game_settings->window_height,
        "Aisling");
        
        // Load game icon.
        Image icon = LoadImage("../assets/images/icon/app_icon.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
        
        // Prevent closing the window with ESC automatically so it can be used for pause.
        SetExitKey(0);
    }
    
void RunGame(Character *player, Audio *game_audio, Settings *game_settings,
        Scene *game_scene, Interactive *game_interactive,
        Dialogue *game_dialogue, Map *game_map,
        Interactable *worldObjects, GameContext *game_context,
        GameState *game_state){
    /* Run the game */
    Dialogue *current_dialogue = game_dialogue;
    Interactable *objectToInteractWith = NULL;

    while (!WindowShouldClose()){
        // Update audio stream.
        UpdateAudio(game_audio);

        // Calculate player hitbox and map size.
        Rectangle playerHitbox = {player->position.x + 75, player->position.y + 50,
                                60, 80};
        
        Vector2 map_size = {(float)game_map->tiled_map->width * game_map->tiled_map->tilewidth,
                            (float)game_map->tiled_map->height * game_map->tiled_map->tileheight};

        objectToInteractWith = NULL;
        for (int i = 0; i < 2; i++){
            // Check collision
            if (CheckCollisionRecs(playerHitbox, worldObjects[i].bounds)){
                worldObjects[i].isActive = true;
                objectToInteractWith = &worldObjects[i];
            } else {
                worldObjects[i].isActive = false;
            }
        }

        // Toggle pause state
        if (IsKeyPressed(KEY_ESCAPE)) {
            *game_state = (*game_state == PAUSE) ? GAMEPLAY : PAUSE;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (*game_state == GAMEPLAY && objectToInteractWith != NULL){
                // Start dialogue
                UnloadDialogue(current_dialogue);
                *current_dialogue = LoadDialogue(objectToInteractWith->dialoguePath);

                if (current_dialogue->line_count > 0){
                    *game_state = DIALOGUE_CUTSCENE;
                    current_dialogue->current_line = 0;
                }
            } else if (*game_state == DIALOGUE_CUTSCENE){
                // Increment first
                current_dialogue->current_line++;

                // If it has reached or passed the line count, go back to gameplay
                if (current_dialogue->current_line >= current_dialogue->line_count){
                    *game_state = GAMEPLAY;
                    current_dialogue->current_line = 0;
                }
            }
        }

        switch(*game_state){
            case MAINMENU:
                UpdateInteractive(game_interactive, game_settings);
                if (game_interactive->is_play_clicked){
                    *game_state = GAMEPLAY;
                }
                if (game_interactive->is_settings_clicked){
                    *game_state = SETTINGS;
                }
                if (game_interactive->is_quit_clicked){
                    return;
                }
                ShowCursor();
                break;
            case GAMEPLAY:
                UpdateCharacter(player, game_settings, map_size, game_map);
                UpdateGameContext(game_context, game_settings, map_size);

                if (player->position.x == 200)
                    PlayScream(game_audio);
                HideCursor();
                break;
            case PAUSE:
                UpdateInteractive(game_interactive, game_settings);
                if (game_interactive->is_play_clicked){
                    *game_state = GAMEPLAY;
                }
                if (game_interactive->is_settings_clicked){
                    *game_state = SETTINGS;
                }
                if (game_interactive->is_quit_clicked){
                    return;
                }
                ShowCursor();
                break;
            case SETTINGS:
                UpdateInteractive(game_interactive, game_settings);
                if (IsKeyPressed(KEY_ESCAPE)){
                    *game_state = MAINMENU;
                }
                ShowCursor();
                break;
            default:
                break;
        }

        // Draw game assets to the screen.
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (*game_state == MAINMENU) {
            DrawMainMenu(game_scene, game_interactive);
        } else if (*game_state == SETTINGS) {
            DrawSettings(game_scene, game_settings, game_interactive);
        } else if (*game_state == PAUSE) {
            DrawPauseMenu(game_scene, game_settings, game_interactive);
        } else {
            BeginMode2D(game_context->camera);
                DrawMap(game_map);
                DrawCharacter(player);
                for (int i = 0; i < 2; i++){
                    Color boxColor = worldObjects[i].isActive ? LIME : GRAY;

                    DrawRectangleRec(worldObjects[i].bounds, boxColor);

                    if (worldObjects[i].isActive){
                        DrawText("!", worldObjects[i].bounds.x + 20,
                                worldObjects[i].bounds.y - 30, 20, RED);
                    }
                }
            EndMode2D();
            
            DrawTexture(game_scene->vignette, 0, 0, WHITE);

            if (*game_state == DIALOGUE_CUTSCENE){
                DrawRectangle(0, GetScreenHeight() - 120, GetScreenWidth(), 120,
                            Fade(BLACK, 0.8f));

                const char *line =
                    current_dialogue->lines[current_dialogue->current_line];
                DrawText(line, GetScreenWidth() / 2 - MeasureText(line, 20) / 2,
                        GetScreenHeight() - 80, 20, WHITE);
                DrawText("Press ENTER to continue", GetScreenWidth() - 150,
                        GetScreenHeight() - 30, 10, GRAY);
            }
        }
        EndDrawing();
    }
}

void EndGame(Audio *game_audio, Character *player, Scene *game_scene,
            Interactive *game_interactive, Map *game_map, Settings *game_settings){
    /* End the game */

    // Save the game data.
    SaveData(player, game_settings);

    // Prepare to stop the game.
    CloseAudio(game_audio);
    CloseCharacter(player);
    CloseScene(game_scene);
    CloseInteractive(game_interactive);
    FreeMap(game_map);

    // Close the game window.
    CloseWindow();
}