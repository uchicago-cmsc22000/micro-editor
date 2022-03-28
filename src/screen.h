/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * screen.h: High-level functions for drawing and manipulating
 *           the editor's screen.
 */

#ifndef SCREEN_H
#define SCREEN_H

/* screen_refresh - Refresh the screen
 *
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void screen_refresh(editor_ctx_t *ctx);


/* screen_set_status_message - Set the status message
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - fmt, ...: printf-style format string and parameters
 * 
 * Returns: Nothing
 */
void screen_set_status_message(editor_ctx_t *ctx, const char *fmt, ...);

#endif /* SCREEN_H */