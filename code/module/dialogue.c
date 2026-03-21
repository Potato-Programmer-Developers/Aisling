/**
 * @file dialogue.c
 * @brief Implementation of the dialogue loading and parsing system.
 * 
 * Handles file I/O for narrative text files, sanitizing strings by 
 * removing newlines and enforcing line limits.
 * 
 * Authors: Cornelius Jabez Lim
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dialogue.h"

/**
 * @brief Opens and parses a dialogue text file.
 * 
 * Reads lines sequentially using fgets. Sanitizes input by truncating
 * newline characters.
 * 
 * @param filename Path to the .txt file.
 * @return Initialized Dialogue struct with loaded lines.
 */
Dialogue LoadDialogue(const char* filename)
{
    Dialogue dialogue = {0};

    FILE* file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Failed to load dialogue file: %s\n", filename);
        return dialogue;
    }

    while (fgets(dialogue.lines[dialogue.line_count], MAX_LINE_LENGTH, file))
    {
        // Remove newline character
        dialogue.lines[dialogue.line_count][strcspn(dialogue.lines[dialogue.line_count], "\n")] = 0;
        dialogue.line_count++;

        // Stop if we reach the maximum number of lines
        if (dialogue.line_count >= MAX_DIALOGUE_LINES)
            break;
    }

    fclose(file);

    // Start at the first line
    dialogue.current_line = 0;

    return dialogue;
}