/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * terminal.c: Lower-level terminal operations.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "common.h"
#include "terminal.h"

/* Used to store the terminal state when the program starts
 * (so we can restore it when it exits). Has to be a global 
 * variable because atexit() doesn't take any parameters */
static struct termios orig_termios;


/*
 * terminal_disable_raw_mode - Disables terminal raw mode
 *
 * Called by atexit(), so takes no parameters and
 * returns nothing, as required by atexit()
 */
void terminal_disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        terminal_die("tcsetattr");
}


/* See terminal.h */
void terminal_enable_raw_mode()
{
    /* Save terminal state and make sure it is restored when we exit */
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        terminal_die("tcgetattr");
    atexit(terminal_disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        terminal_die("tcsetattr");
}

/* See terminal.h */
int terminal_read_key()
{
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            terminal_die("read");
    }

    /* Arrow key processing */

    if (c == '\x1b')
    {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';
        if (seq[0] == '[')
        {
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';
                if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                    case '1':
                        return HOME_KEY;
                    case '3':
                        return DEL_KEY;
                    case '4':
                        return END_KEY;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME_KEY;
                    case '8':
                        return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O')
        {
            switch (seq[1])
            {
            case 'H':
                return HOME_KEY;
            case 'F':
                return END_KEY;
            }
        }
        return '\x1b';
    }
    else
    {

        return c;
    }
}


/* get_cursor_position - Get current position of cursor
 * 
 * Parameters:
 *  - rows, cols: Output parameters to return the position of the cursor
 * 
 * Returns: 0 on success, -1 if the position could not be obtained
 */
int get_cursor_position(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;
    return 0;
}


/* See terminal.h */
int terminal_get_window_size(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        return get_cursor_position(rows, cols);
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}


/* See terminal.h */
void terminal_die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}
