/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * screen.c: High-level functions for drawing and manipulating
 *           the editor's screen.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "editor.h"


/* We define a simple "screen" type that represents the contents of the screen
 * (including escape characters). This is basically just a dynamic string type 
 * that supports one operation: appending. This allows us to write the contents
 * of the screen with a single write() call, instead of character-by-character. */
typedef struct screen
{
    char *buf;
    int len;
} screen_t;


/* Macro to initialize a dynamic string */
#define SCREEN_INIT \
    {               \
        NULL, 0     \
    }


/* screen_append - Append to the screen
 *
 * Parameters:
 *  - screen: The screen
 *  - s: String to append
 *  - len: Length of string to append
 * 
 * Returns: Nothing
 */
void screen_append(screen_t *screen, const char *s, int len)
{
    char *new = realloc(screen->buf, screen->len + len);
    if (new == NULL)
        return;
    memcpy(&new[screen->len], s, len);
    screen->buf = new;
    screen->len += len;
}


/* screen_free - Free the screen
 *
 * Parameters:
 *  - screen: The screen
 * 
 * Returns: Nothing
 */
void screen_free(screen_t *screen)
{
    free(screen->buf);
}


/* screen_scroll - Update the row/column offsets based on the cursor
 *
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void screen_scroll(editor_ctx_t *ctx)
{
    ctx->rx = 0;
    if (ctx->cy < ctx->num_rows)
    {
        ctx->rx = editor_row_cx2rx(&ctx->rows[ctx->cy], ctx->cx);
    }

    if (ctx->cy < ctx->rowoff)
    {
        ctx->rowoff = ctx->cy;
    }
    if (ctx->cy >= ctx->rowoff + ctx->screen_rows)
    {
        ctx->rowoff = ctx->cy - ctx->screen_rows + 1;
    }
    if (ctx->rx < ctx->coloff)
    {
        ctx->coloff = ctx->rx;
    }
    if (ctx->rx >= ctx->coloff + ctx->screen_cols)
    {
        ctx->coloff = ctx->rx - ctx->screen_cols + 1;
    }
}


/* screen_draw_rows - Draws the editor rows
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - screen: Editor screen
 * 
 * Returns: Nothing
 */
void screen_draw_rows(editor_ctx_t *ctx, screen_t *screen)
{
    int y;
    for (y = 0; y < ctx->screen_rows; y++)
    {
        int filerow = y + ctx->rowoff;
        if (filerow >= ctx->num_rows)
        {
            if (ctx->num_rows == 0 && y == ctx->screen_rows / 3)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                                          "Micro editor -- version %s", MICRO_VERSION);
                if (welcomelen > ctx->screen_cols)
                    welcomelen = ctx->screen_cols;
                int padding = (ctx->screen_cols - welcomelen) / 2;
                if (padding)
                {
                    screen_append(screen, "~", 1);
                    padding--;
                }
                while (padding--)
                    screen_append(screen, " ", 1);
                screen_append(screen, welcome, welcomelen);
            }
            else
            {
                screen_append(screen, "~", 1);
            }
        }
        else
        {
            int len = ctx->rows[filerow].rsize - ctx->coloff;
            if (len < 0)
                len = 0;
            if (len > ctx->screen_cols)
                len = ctx->screen_cols;
            screen_append(screen, &ctx->rows[filerow].render[ctx->coloff], len);
        }
        screen_append(screen, "\x1b[K", 3);
        screen_append(screen, "\r\n", 2);
    }
}


/* editor_draw_status_bar - Draw the status bar
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - screen: Editor screen
 * 
 * Returns: Nothing
 */
void screen_draw_status_bar(editor_ctx_t *ctx, screen_t *screen)
{
    screen_append(screen, "\x1b[7m", 4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                       ctx->filename ? ctx->filename : "[No Name]", ctx->num_rows,
                       ctx->dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",
                        ctx->cy + 1, ctx->num_rows);
    if (len > ctx->screen_cols)
        len = ctx->screen_cols;
    screen_append(screen, status, len);
    while (len < ctx->screen_cols)
    {
        if (ctx->screen_cols - len == rlen)
        {
            screen_append(screen, rstatus, rlen);
            break;
        }
        else
        {
            screen_append(screen, " ", 1);
            len++;
        }
    }
    screen_append(screen, "\x1b[m", 3);
    screen_append(screen, "\r\n", 2);
}


/* editor_draw_status_bar - Draw the status bar
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - screen: Editor screen
 * 
 * Returns: Nothing
 */
void screen_draw_message_bar(editor_ctx_t *ctx, screen_t *screen)
{
    screen_append(screen, "\x1b[K", 3);
    int msglen = strlen(ctx->statusmsg);
    if (msglen > ctx->screen_cols)
        msglen = ctx->screen_cols;
    if (msglen && time(NULL) - ctx->statusmsg_time < 5)
        screen_append(screen, ctx->statusmsg, msglen);
}


/* See screen.h */
void screen_refresh(editor_ctx_t *ctx)
{
    screen_scroll(ctx);

    screen_t screen = SCREEN_INIT;

    screen_append(&screen, "\x1b[?25l", 6);
    screen_append(&screen, "\x1b[H", 3);

    screen_draw_rows(ctx, &screen);
    screen_draw_status_bar(ctx, &screen);
    screen_draw_message_bar(ctx, &screen);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (ctx->cy - ctx->rowoff) + 1,
             (ctx->rx - ctx->coloff) + 1);
    screen_append(&screen, buf, strlen(buf));

    screen_append(&screen, "\x1b[?25h", 6);

    write(STDOUT_FILENO, screen.buf, screen.len);

    screen_free(&screen);
}


/* See screen.h */
void screen_set_status_message(editor_ctx_t *ctx, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(ctx->statusmsg, sizeof(ctx->statusmsg), fmt, ap);
    va_end(ap);
    ctx->statusmsg_time = time(NULL);
}
