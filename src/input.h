/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * input.c: Functions for getting input from the user.
 */

#ifndef INPUT_H
#define INPUT_H

/* input_process_keypress - Process a single keypress
 *
 * Parameters:
 *  - ctx: Editor context object
 * 
 * Returns: Nothing
 */
void input_process_keypress(editor_ctx_t *ctx);


/* input_prompt - Prompt the user for a value
 *
 * Print a prompt in the status bar, and wait for the user to
 * type a value.
 *
 * Parameters:
 *  - ctx: Editor context object
 *  - prompt: Prompt to print in the status bar
 * 
 * Returns: Text entered by user
 */
char *input_prompt(editor_ctx_t *ctx, char *prompt, void (*callback)(editor_ctx_t *, char *, int));

#endif /* INPUT_H */