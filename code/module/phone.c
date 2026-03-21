/**
 * @file phone.c
 * @brief Implementation of the in-game mobile phone notification system.
 * 
 * Handles pop-up notifications and a dedicated phone UI for reading
 * and responding to narrative messages.
 * 
 * Authors: Andrew Zhuo
 */

#include "phone.h"
#include <string.h>

/** @brief Resets phone state and clears message buffers. */
void InitPhone(Phone *phone){
    phone->state = PHONE_IDLE;
    phone->notif_timer = 0;
    memset(phone->message, 0, sizeof(phone->message));
    memset(phone->replies, 0, sizeof(phone->replies));
    memset(phone->selected_reply, 0, sizeof(phone->selected_reply));
}

/**
 * @brief Triggers a new message notification.
 * 
 * Sets the notification to auto-dismiss after 5 seconds if not opened.
 */
void TriggerPhoneNotification(Phone *phone, const char *msg, const char *reply1, const char *reply2){
    strncpy(phone->message, msg, sizeof(phone->message) - 1);
    strncpy(phone->replies[0], reply1, sizeof(phone->replies[0]) - 1);
    strncpy(phone->replies[1], reply2, sizeof(phone->replies[1]) - 1);

    // Init 5-second countdown
    phone->notif_timer = 5.0f;
    phone->state = PHONE_NOTIFICATION;
}

/** @brief Updates the notification auto-dismiss timer. */
void UpdatePhone(Phone *phone, float delta){
    if (phone->state == PHONE_NOTIFICATION){
        phone->notif_timer -= delta;
        if (phone->notif_timer <= 0){
            phone->state = PHONE_IDLE;
        }
    }
}

/**
 * @brief Renders the phone UI overlays.
 * 
 * Handles both the small 'New Message' pop-up and the full-screen 
 * messaging interface.
 */
void DrawPhone(Phone *phone){
    if (phone->state == PHONE_IDLE) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (phone->state == PHONE_NOTIFICATION){
        // --- Pop-up Notification (Bottom Right) ---
        int boxWidth = 300;
        int boxHeight = 60;
        int margin = 20;
        Rectangle box = {
            (float)screenWidth - boxWidth - margin,
            (float)screenHeight - boxHeight - margin,
            (float)boxWidth, (float)boxHeight
        };
        
        DrawRectangleRec(box, Fade(BLACK, 0.8f));
        DrawRectangleLinesEx(box, 2, GOLD);
        DrawText("New Message! Press 'R'", box.x + 10, box.y + 20, 20, WHITE);
    } else if (phone->state == PHONE_OPENED || phone->state == PHONE_SHOWING_REPLY){
        // --- Full Phone Interface (Center) ---
        int phoneWidth = 400;
        int phoneHeight = 600;
        Rectangle phoneBox = {
            (float)screenWidth / 2.0f - phoneWidth / 2.0f,
            (float)screenHeight / 2.0f - phoneHeight / 2.0f,
            (float)phoneWidth, (float)phoneHeight
        };

        // UI Frame
        DrawRectangleRec(phoneBox, DARKGRAY);
        DrawRectangleLinesEx(phoneBox, 4, BLACK);
        
        // Status Bar
        DrawRectangle(phoneBox.x, phoneBox.y, phoneBox.width, 40, BLACK);
        DrawText("PHONE", phoneBox.x + phoneBox.width / 2.0f - MeasureText("PHONE", 20) / 2.0f, phoneBox.y + 10, 20, RAYWHITE);

        // Screen area
        Rectangle screen = { phoneBox.x + 10, phoneBox.y + 50, phoneBox.width - 20, phoneBox.height - 180 };
        DrawRectangleRec(screen, Fade(SKYBLUE, 0.2f));
        DrawText(phone->message, screen.x + 20, screen.y + 20, 20, WHITE);

        if (phone->state == PHONE_OPENED){
            // Prompt to respond
            DrawText("Press ENTER to reply", phoneBox.x + 20, phoneBox.y + phoneBox.height - 80, 20, GREEN);
        } else if (phone->state == PHONE_SHOWING_REPLY){
            // Narrative display of sent message
            DrawText("Me:", screen.x + 20, screen.y + 100, 15, GREEN);
            DrawText(phone->selected_reply, screen.x + 20, screen.y + 120, 20, WHITE);
        }
    }
}

/**
 * @brief Handles keyboard input for opening/closing/replying on the phone.
 * 
 * Uses 'R' as the primary interact key and 'ENTER' for replying.
 */
void HandlePhoneInput(Phone *phone){
    if (phone->state == PHONE_IDLE) return;

    if (phone->state == PHONE_NOTIFICATION){
        // Open the message from pop-up
        if (IsKeyPressed(KEY_R)){
            phone->state = PHONE_OPENED;
            return;
        }
    } else if (phone->state == PHONE_OPENED){
        // Simplified reply mechanic (chooses first option)
        if (IsKeyPressed(KEY_ENTER)){
            strncpy(phone->selected_reply, phone->replies[0], sizeof(phone->selected_reply) - 1);
            phone->state = PHONE_SHOWING_REPLY;
        }
    }

    // Toggle close
    if (IsKeyPressed(KEY_R)){
        phone->state = PHONE_IDLE;
    }
}
