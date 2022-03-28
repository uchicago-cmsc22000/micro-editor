/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * editor.h: Provides high-level editor operations, like loading,
 *           saving, inserting a character at the cursor's position, etc.
 */

#ifndef EDITOR_H
#define EDITOR_H

#include <time.h>
#include "row.h"

/* Context object to store global information about the editor */
typedef struct editor_ctx
{
    /* Number of rows in the terminal */
    int screen_rows;

    /* NUmber of columns in the terminal */
    int screen_cols;

    /* Cursor position */
    int cx, cy;

    /* Editor rows */
    erow_t *rows;

    /* Has the file been modified since its last save? */
    int dirty;

    /* Number of editor rows */
    int num_rows;

    /* Row and column offsets (the row/column we're currently scrolled to) */
    int rowoff;
    int coloff;

    /* Index into the rendered editor row
     * In the absence of tabs or characters that are rendered as multiple
     * characters, this field will be equal to cx */
    int rx;

    /* File (if any) that is being edited */
    char *filename;

    /* Status message to be displayed at the bottom of the screen */
    char statusmsg[80];

    /* Time when the status message was added (so we can time it out) */
    time_t statusmsg_time;
} editor_ctx_t;


/* init_editor - Initializes the editor
 *
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void init_editor(editor_ctx_t *ctx);


/* editor_insert_char - Insert character at cursor
 *
 * Inserts a character at the cursor's current position.
 * 
 * Parameters:
 *  - ctx: Editor context object
 *  - c: Character to insert
 * 
 * Returns: Nothing
 */
void editor_insert_char(editor_ctx_t *ctx, int c);


/* editor_insert_newline - Insert a line break at cursor
 * 
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void editor_insert_newline(editor_ctx_t *ctx);


/* editor_delete_char - Delete character at cursor
 *
 * Delete the character at the cursor's current position.
 * 
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void editor_delete_char(editor_ctx_t *ctx);


/* editor_open_file - Opens a file in the editor
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - filename: File to open
 * 
 * Returns: Nothing
 */
void editor_open_file(editor_ctx_t *ctx, char *filename);


/* editor_save_file - Saves the currently open file
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - filename: File to open
 * 
 * Returns: Nothing
 */
void editor_save_file(editor_ctx_t *ctx);


/* editor_find - Search in the editor
 *
 * Prompts the user for a search string, and moves the cursor
 * to the first occurrence of that string.
 *
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void editor_find(editor_ctx_t *ctx);

#endif /* EDITOR_H */