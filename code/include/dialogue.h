/**
 * @file dialogue.h
 * @brief Function prototypes and definitions for the dialogue system.
 * 
 * Defines the structure for managing multi-line dialogue boxes and 
 * handles loading narrative text from external files.
 * 
 * Authors: Cornelius Jabez Lim
 */

#ifndef DIALOGUE_H
#define DIALOGUE_H

#define MAX_DIALOGUE_LINES 100
#define MAX_LINE_LENGTH 256

/**
 * @brief Container for a dialogue sequence.
 * 
 * Stores lines of text and tracks the current reading progress.
 */
typedef struct {
    char lines[MAX_DIALOGUE_LINES][MAX_LINE_LENGTH];          // Buffer for dialogue strings
    int line_count;                                           // Total number of lines loaded
    int current_line;                                         // Index of the currently active line
} Dialogue;

/**
 * @brief Parses a text file into a Dialogue object.
 * 
 * @param filename FS path to the .txt dialogue source.
 * @return Populated Dialogue struct. Empty struct if file load fails.
 */
Dialogue LoadDialogue(const char* filename);

#endif