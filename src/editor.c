/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * editor.c: Provides high-level editor operations, like loading,
 *           saving, inserting a character at the cursor's position, etc.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"
#include "editor.h"
#include "input.h"
#include "screen.h"
#include "terminal.h"


/* See editor.h */
void init_editor(editor_ctx_t *ctx)
{
    if (terminal_get_window_size(&ctx->screen_rows, &ctx->screen_cols) == -1)
        terminal_die("terminal_get_window_size");

    /* Make room for the status bar and the status message*/
    ctx->screen_rows -= 2;

    ctx->cx = 0;
    ctx->cy = 0;
    ctx->rx = 0;

    ctx->num_rows = 0;
    ctx->rows = NULL;

    ctx->dirty = 0;

    ctx->rowoff = 0;
    ctx->coloff = 0;

    ctx->filename = NULL;

    ctx->statusmsg[0] = '\0';
    ctx->statusmsg_time = 0;
}


/* See editor.h */
void editor_insert_char(editor_ctx_t *ctx, int c)
{
    if (ctx->cy == ctx->num_rows)
    {
        editor_row_insert(ctx, ctx->num_rows, "", 0);
    }
    editor_row_insert_char(&ctx->rows[ctx->cy], ctx->cx, c);
    ctx->cx++;
    ctx->dirty++;
}


/* See editor.h */
void editor_insert_newline(editor_ctx_t *ctx)
{
    if (ctx->cx == 0)
    {
        editor_row_insert(ctx, ctx->cy, "", 0);
    }
    else
    {
        erow_t *row = &ctx->rows[ctx->cy];
        editor_row_insert(ctx, ctx->cy + 1, &row->chars[ctx->cx], row->size - ctx->cx);
        row = &ctx->rows[ctx->cy];
        row->size = ctx->cx;
        row->chars[row->size] = '\0';
        editor_row_render(row);
    }
    ctx->cy++;
    ctx->cx = 0;
}


/* See editor.h */
void editor_delete_char(editor_ctx_t *ctx)
{
    if (ctx->cy == ctx->num_rows)
        return;

    if (ctx->cx == 0 && ctx->cy == 0)
        return;

    erow_t *row = &ctx->rows[ctx->cy];
    if (ctx->cx > 0)
    {
        editor_row_delete_char(row, ctx->cx - 1);
        ctx->cx--;
    }
    else
    {
        ctx->cx = ctx->rows[ctx->cy - 1].size;
        editor_row_append_string(&ctx->rows[ctx->cy - 1], row->chars, row->size);
        editor_row_delete(ctx, ctx->cy);
        ctx->cy--;
    }
    ctx->dirty++;
}


/* See editor.h */
void editor_open_file(editor_ctx_t *ctx, char *filename)
{
    free(ctx->filename);
    ctx->filename = strdup(filename);

    FILE *fp = fopen(filename, "r");
    if (!fp)
        terminal_die("fopen");
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1)
    {
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                               line[linelen - 1] == '\r'))
            linelen--;
        editor_row_insert(ctx, ctx->num_rows, line, linelen);
    }
    free(line);
    fclose(fp);
    ctx->dirty = 0;
}


/* See editor.h */
void editor_save_file(editor_ctx_t *ctx)
{
    if (ctx->filename == NULL)
    {
        ctx->filename = input_prompt(ctx, "Save as: %s (ESC to cancel)", NULL);
        if (ctx->filename == NULL)
        {
            screen_set_status_message(ctx, "Save cancelled");
            return;
        }
    }

    int len;
    char *buf = editor_rows_to_string(ctx, &len);
    int fd = open(ctx->filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1)
    {
        if (ftruncate(fd, len) != -1)
        {
            if (write(fd, buf, len) == len)
            {
                close(fd);
                free(buf);
                ctx->dirty = 0;
                screen_set_status_message(ctx, "%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }
    free(buf);

    screen_set_status_message(ctx, "Can't save! I/O error: %s", strerror(errno));
}


/* editor_find_callback - Callback function for input_prompt()
 *
 * Searches for the provided string and moves the cursor
 * to the first occurrence of that string.
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - query: Search term
 *  - key: Key that was pressed
 * 
 * Returns: Nothing
 */
void editor_find_callback(editor_ctx_t *ctx, char *query, int key)
{
    static int last_match = -1;
    static int direction = 1;

    if (key == '\r' || key == '\x1b')
    {
        last_match = -1;
        direction = 1;
        return;
    }
    else if (key == ARROW_RIGHT || key == ARROW_DOWN)
    {
        direction = 1;
    }
    else if (key == ARROW_LEFT || key == ARROW_UP)
    {
        direction = -1;
    }
    else
    {
        last_match = -1;
        direction = 1;
    }

    if (last_match == -1)
        direction = 1;
    int current = last_match;

    for (int i = 0; i < ctx->num_rows; i++)
    {
        current += direction;
        if (current == -1)
            current = ctx->num_rows - 1;
        else if (current == ctx->num_rows)      
            current = 0;

        erow_t *row = &ctx->rows[current];
        char *match = strstr(row->render, query);
        if (match)
        {
            last_match = current;
            ctx->cy = current;
            ctx->cx = editor_row_rx2cx(row, match - row->render);
            ctx->rowoff = ctx->num_rows;
            break;
        }
    }
}


/* See editor.h */
void editor_find(editor_ctx_t *ctx)
{
    int saved_cx = ctx->cx;
    int saved_cy = ctx->cy;
    int saved_coloff = ctx->coloff;
    int saved_rowoff = ctx->rowoff;

    char *query = input_prompt(ctx, "Search: %s (Use ESC/Arrows/Enter)", editor_find_callback);

    if (query)
    {
        free(query);
    }
    else
    {
        ctx->cx = saved_cx;
        ctx->cy = saved_cy;
        ctx->coloff = saved_coloff;
        ctx->rowoff = saved_rowoff;
    }
}
