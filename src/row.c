/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * row.c: Lower-level operations on individual "rows" of the
 *        editor (a "row" corresponds to a line in the file we are editing)  
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "row.h"
#include "editor.h"


/* See row.h */
int editor_row_cx2rx(erow_t *row, int cx)
{
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++)
    {
        if (row->chars[j] == '\t')
            rx += (MICRO_TAB_STOP - 1) - (rx % MICRO_TAB_STOP);
        rx++;
    }
    return rx;
}


/* See row.h */
int editor_row_rx2cx(erow_t *row, int rx)
{
    int cur_rx = 0;
    int cx;
    for (cx = 0; cx < row->size; cx++)
    {
        if (row->chars[cx] == '\t')
            cur_rx += (MICRO_TAB_STOP - 1) - (cur_rx % MICRO_TAB_STOP);
        cur_rx++;
        if (cur_rx > rx)
            return cx;
    }
    return cx;
}


/* See row.h */
void editor_row_render(erow_t *row)
{
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t')
            tabs++;
    free(row->render);
    row->render = malloc(row->size + tabs * (MICRO_TAB_STOP - 1) + 1);
    int idx = 0;
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % MICRO_TAB_STOP != 0)
                row->render[idx++] = ' ';
        }
        else
        {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}


/* See row.h */
void editor_row_insert(editor_ctx_t *ctx, int at, char *s, size_t len)
{
    if (at < 0 || at > ctx->num_rows)
        return;

    ctx->rows = realloc(ctx->rows, sizeof(erow_t) * (ctx->num_rows + 1));
    memmove(&ctx->rows[at + 1], &ctx->rows[at], sizeof(erow_t) * (ctx->num_rows - at));

    ctx->rows[at].size = len;
    ctx->rows[at].chars = malloc(len + 1);
    memcpy(ctx->rows[at].chars, s, len);
    ctx->rows[at].chars[len] = '\0';

    ctx->rows[at].rsize = 0;
    ctx->rows[at].render = NULL;
    editor_row_render(&ctx->rows[at]);

    ctx->num_rows++;
    ctx->dirty++;
}


/* See row.h */
void editor_row_free(erow_t *row)
{
    free(row->render);
    free(row->chars);
}


/* See row.h */
void editor_row_delete(editor_ctx_t *ctx, int row_idx)
{
    if (row_idx < 0 || row_idx >= ctx->num_rows)
        return;
    editor_row_free(&ctx->rows[row_idx]);
    memmove(&ctx->rows[row_idx], &ctx->rows[row_idx + 1], sizeof(erow_t) * (ctx->num_rows - row_idx - 1));
    ctx->num_rows--;
    ctx->dirty++;
}


/* See row.h */
void editor_row_insert_char(erow_t *row, int at, int c)
{
    if (at < 0 || at > row->size)
        at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editor_row_render(row);
}


/* See row.h */
void editor_row_append_string(erow_t *row, char *s, size_t len)
{
    /* Reallocate memory so appended string fits in row */
    row->chars = realloc(row->chars, row->size + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editor_row_render(row);
}


/* See row.h */
void editor_row_delete_char(erow_t *row, int at)
{
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editor_row_render(row);
}


/* See row.h */
char *editor_rows_to_string(editor_ctx_t *ctx, int *buflen)
{
    int totlen = 0;
    int j;
    for (j = 0; j < ctx->num_rows; j++)
        totlen += ctx->rows[j].size + 1;
    *buflen = totlen;
    char *buf;
    char *p = buf;
    for (j = 0; j < ctx->num_rows; j++)
    {
        memcpy(p, ctx->rows[j].chars, ctx->rows[j].size);
        p += ctx->rows[j].size;
        *p = '\n';
        p++;
    }
    return buf;
}
