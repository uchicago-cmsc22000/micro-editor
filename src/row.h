/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * row.h: Lower-level operations on individual "rows" of the
 *        editor (a "row" corresponds to a line in the file we are editing)  
 */

#ifndef ROW_H
#define ROW_H

#include <time.h>

/* Forward declaration of editor context */
typedef struct editor_ctx editor_ctx_t;

/* An "editor row" (a line of text) */
typedef struct erow
{
    /* The actual line of text */
    int size;
    char *chars;

    /* The rendered version of that line */
    int rsize;
    char *render;
} erow_t;

/* editor_row_cx2rx 
 * 
 * Concerts the cursor position to a position into
 * the rendered row
 * 
 * Parameters:
 *  - row: Editor row
 *  - cx: Cursor position to convert
 * 
 * Returns: An index into the rendered row
 */
int editor_row_cx2rx(erow_t *row, int cx);


/* editor_row_rx2cx 
 * 
 * Concerts a position in the rendered row into a
 * cursor position
 * 
 * Parameters:
 *  - row: Editor row
 *  - rx: Rendered row position
 * 
 * Returns: A cursor position in the row
 */
int editor_row_rx2cx(erow_t *row, int rx);

/* editor_row_render - Render an editor row
 * 
 * Take the raw content of an editor row and produce the rendered
 * version (currently replaces tabs with 4 spaces)
 * 
 * Parameters:
 *  - row: Editor row to render
 * 
 * Returns: nothing
 */
void editor_row_render(erow_t *row);


/* editor_row_insert - Insert a new editor row
 * 
 * Allocate space for a new editor row, and copy the given
 * string as a new editor row, and insert it at the specified
 * index.
 * 
 * Parameters:
 *  - ctx: Editor context
 *  - at: Row index to insert the new row at
 *  - s: String contents of new row
 *  - len: Length of string
 * 
 * Returns: nothing
 */
void editor_row_insert(editor_ctx_t *ctx, int at, char *s, size_t len);


/* editor_row_free - Free a row
 * 
 * Parameters:
 *  - row: Editor row
 * 
 * Returns: nothing
 */
void editor_row_free(erow_t *row);


/* editor_row_delete - Delete a row
 *
 * Deletes a row, and shifts all subsequent rows up one row.
 * 
 * Parameters:
 *  - ctx: Editor context
 *  - row_idx: Index of row to delete
 * 
 * Returns: nothing
 */
void editor_row_delete(editor_ctx_t *ctx, int row_idx);


/* editor_row_insert_char - Inserts a character in a row
 * 
 * Parameters:
 *  - row: Editor row
 *  - at: Position in the row
 *  - c: Character to insert
 * 
 * Returns: nothing
 */
void editor_row_insert_char(erow_t *row, int at, int c);


/* editor_row_append_string - Append a string at the end of a row
 * 
 * Parameters:
 *  - row: Editor row
 *  - s: String to append
 *  - len: Length of the string to append
 * 
 * Returns: nothing
 */
void editor_row_append_string(erow_t *row, char *s, size_t len);


/* editor_row_delete_char - Deletes a character in a row
 * 
 * Parameters:
 *  - row: Editor row
 *  - at: Position in the row
 * 
 * Returns: nothing
 */
void editor_row_delete_char(erow_t *row, int at);


/* editor_rows_to_string - Convert the editor rows to a single string
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - buflen: Output parameter to return the length of the string
 * 
 * Returns: Editor rows as a single string
 */
char *editor_rows_to_string(editor_ctx_t *ctx, int *buflen);


#endif /* ROW_H */