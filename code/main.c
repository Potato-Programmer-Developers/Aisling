/* This file contains the main logic for the game. */

#include "raylib.h"
#include "settings.h"
#include "mc.h"

void init_game();
void run_game(mc player);
void check_movement(mc* player);

int main(void){
    init_game();
    mc player = mc_init();
    run_game(player);

    return 0;
}

void init_game(){
    // Prepare and initialize the game windows.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    SetTargetFPS(fps);
    InitWindow(window_width, window_height, "Top Secret");
}

void run_game(mc player){
    while (!WindowShouldClose()){
        // Draw
        check_movement(&player);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(player.character, player.position.x, player.position.y, WHITE);
        mc_update(&player);
        EndDrawing();
    }

    CloseWindow();
}

void check_movement(mc* player){
    if (IsKeyDown(KEY_LEFT)){
        player->moving_left = true;
    } else{
        player->moving_left = false;
    }
    if (IsKeyDown(KEY_RIGHT)){
        player->moving_right = true;
    } else{
        player->moving_right = false;
    }
    if (IsKeyDown(KEY_UP)){
        player->moving_up = true;
    } else{
        player->moving_up = false;
    }
    if (IsKeyDown(KEY_DOWN)){
        player->moving_down = true;
    } else{
        player->moving_down = false;
    }
}