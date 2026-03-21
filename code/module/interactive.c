/*
This file contains the implementation of the interactive module.

Module made by Andrew Zhuo.
*/

#include "interactive.h"
#include "raylib.h"
#include "raymath.h"

Interactive InitInteractive(Settings* game_settings){
    /* Initialize the interactive elements. */
    Interactive new_interactive = {0};

    // Load button textures.
    new_interactive.new_game_button = LoadTexture("../assets/images/buttons/new_game.png");
    new_interactive.continue_button = LoadTexture("../assets/images/buttons/continue.png");
    new_interactive.main_menu_button = LoadTexture("../assets/images/buttons/main_menu.png");
    new_interactive.settings_button = LoadTexture("../assets/images/buttons/settings.png");
    new_interactive.quit_button = LoadTexture("../assets/images/buttons/quit.png");

    // Initialize slider dimensions.
    new_interactive.bar_width = 400.0f;
    new_interactive.bar_height = 10.0f;
    new_interactive.knob_width = 20.0f;
    new_interactive.knob_height = 30.0f;

    // Calculate initial layout
    UpdateInteractiveLayout(&new_interactive, MAINMENU);
    
    return new_interactive;
}

void UpdateInteractiveLayout(Interactive* interactive, int game_state){
    /* Update the layout of interactive elements based on current screen size and game state. */
    float screen_width = (float)GetScreenWidth();
    float screen_height = (float)GetScreenHeight();
    float button_spacing = 20.0f;
    float slot_h = (float)interactive->new_game_button.height;
    float total_h = 4 * slot_h + 3 * button_spacing;
    float start_y = screen_height / 2.0f - total_h / 2.0f;

    if (game_state == MAINMENU){
        // Main Menu Stack: New Game, Continue, Settings, Quit
        interactive->new_game_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->new_game_button.width / 2.0f,
            start_y,
            (float)interactive->new_game_button.width,
            slot_h
        };
        interactive->continue_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->continue_button.width / 2.0f,
            start_y + 1 * (slot_h + button_spacing),
            (float)interactive->continue_button.width,
            slot_h
        };
        interactive->settings_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->settings_button.width / 2.0f,
            start_y + 2 * (slot_h + button_spacing),
            (float)interactive->settings_button.width,
            slot_h
        };
        interactive->quit_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->quit_button.width / 2.0f,
            start_y + 3 * (slot_h + button_spacing),
            (float)interactive->quit_button.width,
            slot_h
        };
    } else if (game_state == PAUSE){
        // Pause Menu Stack: Continue, Settings, Main Menu, Quit
        interactive->continue_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->continue_button.width / 2.0f,
            start_y,
            (float)interactive->continue_button.width,
            slot_h
        };
        interactive->settings_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->settings_button.width / 2.0f,
            start_y + 1 * (slot_h + button_spacing),
            (float)interactive->settings_button.width,
            slot_h
        };
        interactive->main_menu_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->main_menu_button.width / 2.0f,
            start_y + 2 * (slot_h + button_spacing),
            (float)interactive->main_menu_button.width,
            slot_h
        };
        interactive->quit_bounds = (Rectangle){
            screen_width / 2.0f - (float)interactive->quit_button.width / 2.0f,
            start_y + 3 * (slot_h + button_spacing),
            (float)interactive->quit_button.width,
            slot_h
        };
    }

    // Slider logic
    interactive->volume_slider_bar = (Rectangle){
        screen_width / 2.0f - interactive->bar_width / 2.0f,
        screen_height / 2.0f - interactive->bar_height / 2.0f,
        interactive->bar_width,
        interactive->bar_height
    };
}

void UpdateInteractive(Interactive* interactive, Settings* game_settings){
    /* Update the interactive elements. */

    // Get mouse position
    Vector2 mouse_position = GetMousePosition();

    // Reset clicked flags
    interactive->is_new_game_clicked = false;
    interactive->is_continue_clicked = false;
    interactive->is_main_menu_clicked = false;
    interactive->is_settings_clicked = false;
    interactive->is_quit_clicked = false;

    // Check hover and clicks
    interactive->is_new_game_hovered = CheckCollisionPointRec(mouse_position, interactive->new_game_bounds);
    interactive->is_continue_hovered = CheckCollisionPointRec(mouse_position, interactive->continue_bounds);
    interactive->is_main_menu_hovered = CheckCollisionPointRec(mouse_position, interactive->main_menu_bounds);
    interactive->is_settings_hovered = CheckCollisionPointRec(mouse_position, interactive->settings_bounds);
    interactive->is_quit_hovered = CheckCollisionPointRec(mouse_position, interactive->quit_bounds);

    if (interactive->is_new_game_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_new_game_clicked = true;
    if (interactive->is_continue_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_continue_clicked = true;
    if (interactive->is_main_menu_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_main_menu_clicked = true;
    if (interactive->is_settings_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_settings_clicked = true;
    if (interactive->is_quit_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) interactive->is_quit_clicked = true;


    // Current volume position
    float knob_x = interactive->volume_slider_bar.x + (game_settings->game_volume * interactive->bar_width / 100.0f);
    
    interactive->volume_slider_knob = (Rectangle){
        knob_x - interactive->knob_width / 2.0f,
        interactive->volume_slider_bar.y + interactive->bar_height / 2.0f - interactive->knob_height / 2.0f,
        interactive->knob_width,
        interactive->knob_height
    };

    // Check if the mouse is clicking the volume slider
    if (CheckCollisionPointRec(mouse_position, interactive->volume_slider_knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        interactive->is_volume_moving = true;
    }

    // Check if the mouse is releasing the volume slider
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
        interactive->is_volume_moving = false;
    }

    // Update volume if the mouse is moving the volume slider
    if (interactive->is_volume_moving){
        float new_knob_x = mouse_position.x;
        
        // Clamp knob to bar bounds
        new_knob_x = Clamp(new_knob_x, interactive->volume_slider_bar.x, interactive->volume_slider_bar.x + interactive->bar_width);

        // Update settings volume
        game_settings->game_volume = (new_knob_x - interactive->volume_slider_bar.x) / interactive->bar_width * 100.0f;
        SetMasterVolume(game_settings->game_volume / 100.0f);
        
        // Update knob position
        interactive->volume_slider_knob.x = new_knob_x - interactive->knob_width / 2.0f;
    }
}

void CloseInteractive(Interactive* interactive){
    /* Close the interactive elements. */
    UnloadTexture(interactive->new_game_button);
    UnloadTexture(interactive->continue_button);
    UnloadTexture(interactive->main_menu_button);
    UnloadTexture(interactive->settings_button);
    UnloadTexture(interactive->quit_button);
}