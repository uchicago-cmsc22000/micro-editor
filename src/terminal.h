/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * terminal.h: Lower-level terminal operations.
 */

#ifndef TERMINAL_H
#define TERMINAL_H

/*
 * terminal_enable_raw_mode - Enables terminal raw mode
 * 
 * In raw mode:
 * - The terminal won't echo everything we type,
 * - We will read from the terminal byte by byte, not line by line.
 * - We will ignore a variety of Control key combinations
 * - We disable post-processing of output
 * - Set a variety of legacy flags
 * - Set a timeout for read()
 * 
 * Parameters: None
 * 
 * Returns: Nothing
 */
void terminal_enable_raw_mode();


/* terminal_read_key - Read a single key from the terminal
 * 
 * For ASCII characters, this returns the byte value of the key.
 * For other keys (e.g., the arrow keys) it will return
 * a special integer value.
 * 
 * Parameters: none
 * 
 * Returns: Integer value of keypress
 */
int terminal_read_key();


/* terminal_get_window_size - Returns size of terminal
 * 
 * Parameters:
 *  - rows, cols: Output parameters to return the number of
 *    rows and columns in the terminal.
 * 
 * Returns: 0 on success, -1 if the size of the terminal could not be obtained
 */
int terminal_get_window_size(int *rows, int *cols);


/* terminal_die - Critical error handling
 * 
 * When a critical error happens, we call this function to 
 * clear the screen, print an error message and exit gracefully.
 * 
 * Parameters:
 *  - s: Message
 * 
 * Returns: Nothing
 */
void terminal_die(const char *s);

#endif /* TERMINAL_H */