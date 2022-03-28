/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * input.c: Functions for getting input from the user.
 */

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "common.h"
#include "terminal.h"
#include "editor.h"
#include "screen.h"

/* editor_move_cursor - Moves the cursor based on keypresses
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - key: key that was pressed
 * 
 * Returns: Nothing
 */
void editor_move_cursor(editor_ctx_t *ctx, int key)
{
    erow_t *row = (ctx->cy >= ctx->num_rows) ? NULL : &ctx->rows[ctx->cy];

    switch (key)
    {
    case ARROW_LEFT:
        if (ctx->cx != 0)
        {
            ctx->cx--;
        }
        else if (ctx->cy > 0)
        {
            ctx->cy--;
            ctx->cx = ctx->rows[ctx->cy].size;
        }
        break;
    case ARROW_RIGHT:
        if (row && ctx->cx < row->size)
        {
            ctx->cx++;
        }
        else if (row && ctx->cx == row->size)
        {
            ctx->cy++;
            ctx->cx = 0;
        }
        break;
    case ARROW_UP:
        if (ctx->cy != 0)
        {
            ctx->cy--;
        }
        break;
    case ARROW_DOWN:
        if (ctx->cy < ctx->num_rows)
        {
            ctx->cy++;
        }
        break;
    }

    /* Snap cursor to end of line */
    row = (ctx->cy >= ctx->num_rows) ? NULL : &ctx->rows[ctx->cy];
    int rowlen = row ? row->size : 0;
    if (ctx->cx > rowlen)
    {
        ctx->cx = rowlen;
    }
}


/* See input.h */
void input_process_keypress(editor_ctx_t *ctx)
{
    static int quit_times = MICRO_QUIT_TIMES;

    int c = terminal_read_key();
    switch (c)
    {
    case '\r':
        editor_insert_newline(ctx);
        break;

    /* Ctrl-q: Exit  */
    case CTRL_KEY('q'):
        if (ctx->dirty && quit_times > 0)
        {
            screen_set_status_message(ctx, "WARNING!!! File has unsaved changes. "
                                           "Press Ctrl-Q %d more times to quit.",
                                      quit_times);
            quit_times--;
            return;
        }
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;

    case CTRL_KEY('s'):
        editor_save_file(ctx);
        break;

    /* Moving the cursor */
    case HOME_KEY:
        ctx->cx = 0;
        break;
    case END_KEY:
        if (ctx->cy < ctx->num_rows)
            ctx->cx = ctx->rows[ctx->cy].size;
        break;

    case CTRL_KEY('f'):
        editor_find(ctx);
        break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (c == DEL_KEY)
            editor_move_cursor(ctx, ARROW_RIGHT);
        editor_delete_char(ctx);
        break;

    case PAGE_UP:
    case PAGE_DOWN:
    {
        if (c == PAGE_UP)
        {
            ctx->cy = ctx->rowoff;
        }
        else if (c == PAGE_DOWN)
        {
            ctx->cy = ctx->rowoff + ctx->screen_rows - 1;
            if (ctx->cy > ctx->num_rows)
                ctx->cy = ctx->num_rows;
        }

        int times = ctx->screen_rows;
        while (times--)
            editor_move_cursor(ctx, c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
    break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editor_move_cursor(ctx, c);
        break;

    case CTRL_KEY('l'):
    case '\x1b':
        break;

    default:
        editor_insert_char(ctx, c);
        break;
    }

    quit_times = MICRO_QUIT_TIMES;
}


/* See input.h */
char *input_prompt(editor_ctx_t *ctx, char *prompt, void (*callback)(editor_ctx_t *, char *, int))
{
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    size_t buflen = 0;
    buf[0] = '\0';
    while (1)
    {
        screen_set_status_message(ctx, prompt, buf);
        screen_refresh(ctx);

        int c = terminal_read_key();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE)
        {
            if (buflen != 0)
                buf[--buflen] = '\0';
        }
        else if (c == '\x1b')
        {
            screen_set_status_message(ctx, "");
            if (callback)
                callback(ctx, buf, c);
            free(buf);
            return NULL;
        }
        else if (c == '\r')
        {
            if (buflen != 0)
            {
                screen_set_status_message(ctx, "");
                if (callback)
                    callback(ctx, buf, c);
                return buf;
            }
        }
        else if (!iscntrl(c) && c < 128)
        {
            if (buflen == bufsize - 1)
            {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }

        if (callback)
            callback(ctx, buf, c);
    }
}