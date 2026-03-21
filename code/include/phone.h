/**
 * @file phone.h
 * @brief In-game phone system for narrative messages and player choices.
 * 
 * This module manages a mock smartphone interface that can trigger
 * notifications, display messages, and handle player branching replies.
 * 
 * Authors: Andrew Zhuo
 */

#ifndef PHONE_H
#define PHONE_H

#include "raylib.h"

/**
 * @brief Defines the visual and logical state of the phone system.
 */
typedef enum {
    PHONE_IDLE,             // Hidden/Inactive state
    PHONE_NOTIFICATION,     // Showing a pop-up alert
    PHONE_OPENED,           // Full-screen messaging interface active
    PHONE_SHOWING_REPLY     // Displaying the player's chosen response
} PhoneState;

/**
 * @brief Container for phone data and active message state.
 */
typedef struct {
    char message[128];                 // The content of the current incoming message
    char replies[2][64];               // Two possible response options for the player
    char selected_reply[64];           // The reply chosen by the player
    PhoneState state;                  // Current status of the phone UI
    float notif_timer;                 // Internal timer for auto-hiding notifications
} Phone;

/**
 * @brief Resets phone data to a clean, idle state.
 * @param phone Pointer to the phone structure.
 */
void InitPhone(Phone *phone);

/**
 * @brief Triggers a new incoming message alert.
 * @param phone Pointer to the phone.
 * @param msg The message body.
 * @param reply1 The first reply option.
 * @param reply2 The second reply option.
 */
void TriggerPhoneNotification(
    Phone *phone,
    const char *msg,
    const char *reply1,
    const char *reply2
);

/**
 * @brief Updates phone timers and auto-state transitions.
 * @param phone Pointer to the phone.
 * @param delta Time since last frame.
 */
void UpdatePhone(Phone *phone, float delta);

/**
 * @brief Renders the phone UI (notification or full screen) to the screen.
 * @param phone Pointer to the phone to draw.
 */
void DrawPhone(Phone *phone);

/**
 * @brief Handles player input for selecting replies and opening/closing the phone.
 * @param phone Pointer to the phone.
 */
void HandlePhoneInput(Phone *phone);

#endif
