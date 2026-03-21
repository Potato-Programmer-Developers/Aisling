/**
 * @file interactive.c
 * @brief Implementation of the UI system (Buttons and Sliders).
 * 
 * Handles layout calculations based on screen state and processes
 * mouse interaction for menus and settings.
 * 
 * Authors: Andrew Zhuo
 */

#include "interactive.h"
#include "raylib.h"
#include "raymath.h"

/**
 * @brief Loads common button textures and sets default slider dimensions.
 */
Interactive InitInteractive(Settings* game_settings){
    Interactive new_interactive = {0};

    // 1. Resource Loading
    new_interactive.new_game_button = LoadTexture("../assets/images/buttons/new_game.png");
    new_interactive.continue_button = LoadTexture("../assets/images/buttons/continue.png");
    new_interactive.main_menu_button = LoadTexture("../assets/images/buttons/main_menu.png");
    new_interactive.settings_button = LoadTexture("../assets/images/buttons/settings.png");
    new_interactive.quit_button = LoadTexture("../assets/images/buttons/quit.png");

    // 2. Geometry Setup (Constants)
    new_interactive.bar_width = 400.0f;
    new_interactive.bar_height = 10.0f;
    new_interactive.knob_width = 20.0f;
    new_interactive.knob_height = 30.0f;

    // 3. Initial Layout pass
    UpdateInteractiveLayout(&new_interactive, MAINMENU);
    
    return new_interactive;
}

/**
 * @brief Recalculates screen-space rectangles for all buttons.
 * 
 * Positions buttons in a vertical stack in the center of the screen.
 * Differing logic for MAINMENU vs PAUSE to prioritize specific buttons.
 */
void UpdateInteractiveLayout(Interactive* interactive, int game_state){
    float screen_width = (float)GetScreenWidth();
    float screen_height = (float)GetScreenHeight();
    float button_spacing = 20.0f;
    float slot_h = (float)interactive->new_game_button.height;
    
    // Calculate total height of the button stack to center it vertically
    float total_h = 4 * slot_h + 3 * button_spacing;
    float start_y = screen_height / 2.0f - total_h / 2.0f;

    if (game_state == MAINMENU){
        // Order: New Game -> Continue -> Settings -> Quit
        interactive->new_game_bounds = (Rectangle){ screen_width / 2.0f - (float)interactive->new_game_button.width / 2.0f, start_y, (float)interactive->new_game_button.width, slot_h };
        interactive->continue_bounds = (Rectangle){ screen_width / 2.0f - (float)interactive->continue_button.width / 2.0f, start_y + 1 * (slot_h + button_spacing), (float)interactive->continue_button.width, slot_h };
        interactive->settings_bounds = (Rectangle){ screen_width / 2.0f - (float)interactive->settings_button.width / 2.0f, start_y + 2 * (slot_h + button_spacing), (float)interactive->settings_button.width, slot_h };
        interactive->quit_bounds     = (Rectangle){ screen_width / 2.0f - (float)interactive->quit_button.width / 2.0f, start_y + 3 * (slot_h + button_spacing), (float)interactive->quit_button.width, slot_h };
    } else if (game_state == PAUSE){
        // Order: Continue -> Settings -> Main Menu -> Quit
        interactive->continue_bounds  = (Rectangle){ screen_width / 2.0f - (float)interactive->continue_button.width / 2.0f, start_y, (float)interactive->continue_button.width, slot_h };
        interactive->settings_bounds  = (Rectangle){ screen_width / 2.0f - (float)interactive->settings_button.width / 2.0f, start_y + 1 * (slot_h + button_spacing), (float)interactive->settings_button.width, slot_h };
        interactive->main_menu_bounds = (Rectangle){ screen_width / 2.0f - (float)interactive->main_menu_button.width / 2.0f, start_y + 2 * (slot_h + button_spacing), (float)interactive->main_menu_button.width, slot_h };
        interactive->quit_bounds      = (Rectangle){ screen_width / 2.0f - (float)interactive->quit_button.width / 2.0f, start_y + 3 * (slot_h + button_spacing), (float)interactive->quit_button.width, slot_h };
    }

    // Volume Slider placement (always centered)
    interactive->volume_slider_bar = (Rectangle){ screen_width / 2.0f - interactive->bar_width / 2.0f, screen_height / 2.0f - interactive->bar_height / 2.0f, interactive->bar_width, interactive->bar_height };
}

/**
 * @brief Processes mouse interaction and updates state flags.
 * 
 * Handles button hovering, clicking, and the click-and-drag logic for 
 * the settings volume slider.
 */
void UpdateInteractive(Interactive* interactive, Settings* game_settings){
    Vector2 mouse_position = GetMousePosition();

    // Reset clicked triggers (latches)
    interactive->is_new_game_clicked = false;
    interactive->is_continue_clicked = false;
    interactive->is_main_menu_clicked = false;
    interactive->is_settings_clicked = false;
    interactive->is_quit_clicked = false;

    // --- Phase 1: Button Interaction ---
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

    // --- Phase 2: Slider Interaction ---
    // 1. Calculate knob position based on current settings (percentage mapped to width)
    float knob_x = interactive->volume_slider_bar.x + (game_settings->game_volume * interactive->bar_width / 100.0f);
    
    interactive->volume_slider_knob = (Rectangle){
        knob_x - interactive->knob_width / 2.0f,
        interactive->volume_slider_bar.y + interactive->bar_height / 2.0f - interactive->knob_height / 2.0f,
        interactive->knob_width, interactive->knob_height
    };

    // 2. State transition: Drag Start
    if (CheckCollisionPointRec(mouse_position, interactive->volume_slider_knob) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        interactive->is_volume_moving = true;
    }

    // 3. State transition: Drag Stop
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) interactive->is_volume_moving = false;

    // 4. Transform Logic: Dragging
    if (interactive->is_volume_moving){
        float new_knob_x = mouse_position.x;
        new_knob_x = Clamp(new_knob_x, interactive->volume_slider_bar.x, interactive->volume_slider_bar.x + interactive->bar_width);

        // Map x-position back to 0-100 range
        game_settings->game_volume = (new_knob_x - interactive->volume_slider_bar.x) / interactive->bar_width * 100.0f;
        
        // Immediate Feedback: update Raylib master volume
        SetMasterVolume(game_settings->game_volume / 100.0f);
        
        // Visual sync
        interactive->volume_slider_knob.x = new_knob_x - interactive->knob_width / 2.0f;
    }
}

/**
 * @brief Unloads all pixel data for UI buttons from VRAM.
 */
void CloseInteractive(Interactive* interactive){
    UnloadTexture(interactive->new_game_button);
    UnloadTexture(interactive->continue_button);
    UnloadTexture(interactive->main_menu_button);
    UnloadTexture(interactive->settings_button);
    UnloadTexture(interactive->quit_button);
}