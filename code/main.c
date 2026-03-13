/* This file contains the main logic for the game. */

#include "raylib.h"
#include "settings.h"
#include "character.h"
#include "audio.h"

typedef enum {
    GAMEPLAY,
    PAUSE,
    GAMEOVER
} GameState;

void InitGame(Settings* game_settings);
void RunGame(Character* player, Audio* game_audio, Settings* game_settings);

int main(void){
    // Initialize the game.
    Settings game_settings = InitSettings();
    InitGame(&game_settings);

    // Load game resources.
    Character player = InitCharacter(&game_settings);
    Audio game_audio = InitAudio(&game_settings);

    // Run the game.
    RunGame(&player, &game_audio, &game_settings);
    return 0;
}

void InitGame(Settings* game_settings){
    // Prepare and initialize the game windows.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(game_settings->fps);
    InitWindow(game_settings->window_width, game_settings->window_height, "Aisling");
    Image icon = LoadImage("../assets/images/icon/app_icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetExitKey(0); // Prevent closing the window with ESC automatically so we can use it for pause
}

void RunGame(Character* player, Audio* game_audio, Settings* game_settings){
    GameState game_state = GAMEPLAY;
    
    while (!WindowShouldClose()){
        // Update audio stream.
        UpdateAudio(game_audio);
        
        // Toggle pause state
        if (IsKeyPressed(KEY_ESCAPE)){
            if (game_state == GAMEPLAY) {
                game_state = PAUSE;
            } else if (game_state == PAUSE) {
                game_state = GAMEPLAY;
            }
        }
        
        if (game_state == GAMEPLAY){
            if (player->position.x == 200){
                PlayScream(game_audio);
            }
            UpdateCharacter(player, game_settings);
            HideCursor();
        } else if (game_state == PAUSE){
            ShowCursor();
        }
        
        // Draw game assets to the screen.
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawCharacter(player);
        
        if (game_state == PAUSE) {
            DrawText("PAUSED", game_settings->window_width / 2 - MeasureText("PAUSED", 40) / 2, game_settings->window_height / 2 - 20, 40, LIGHTGRAY);
        }

        EndDrawing();
    }

    // Prepare to stop the game.
    CloseAudio(game_audio);
    CloseCharacter(player);

    // Close the game window.
    CloseWindow();
}