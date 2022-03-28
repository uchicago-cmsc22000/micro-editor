/*
 * micro - A minimal text editor
 * 
 * Based on kilo: https://viewsourcecode.org/snaptoken/kilo/ 
 * 
 * common.h: Common definitions shared by multiple files.
 */

#ifndef COMMON_H
#define COMMON_H

#define MICRO_VERSION "0.220.2021"
#define MICRO_TAB_STOP (4)
#define MICRO_QUIT_TIMES (3)

#define CTRL_KEY(k) ((k)&0x1f)

typedef enum
{
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
} editor_key_t;

#endif /* COMMON_H */