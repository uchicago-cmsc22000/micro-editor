# The `micro` Text Editor

This is our version of the `kilo` editor, a simple
but functional text editor that can be implemented in about 1,000 lines of C
code; if you're interested, you can see a step-by-step guide to writing this
editor here: https://viewsourcecode.org/snaptoken/kilo/ 

`micro` largely follows the same structure as the `kilo` code, except we
divided it into multiple modules and documented the code following the CS 220 style guide.

## Building and Running

You can build the `micro` editor by running the following:

    cmake -B build/
    make -C build/

This will generate a `micro` executable inside a `build/` directory.
You can run the editor like this:

    build/micro

This will open the editor with a blank file. 

Or like this:

    build/micro foobar.txt

This will open file `foobar.txt` (Note: the file must exist already)

Once you've opened `micro`, you can use the arrows keys to move around,
and you can type to edit the file. You can quit the editor
by pressing Ctrl-Q (if you modified the file, you'll have to press it three
times to confirm you want to exit without saving).

Careful: the version of `micro` in this repository is intentionally buggy.
For example, trying to save the file (by pressing Ctrl-S) will cause the editor
to crash.

## Code Organization

The `micro` code is divided into the following files:

- `main.c`: Contains the main() function for the editor, which involves a
  simple event loop that calls functions in other modules.
- `editor.c`/`editor.h`: Provides high-level editor operations, like loading,
  saving, inserting a character at the cursor's position, etc.
- `row.c`/`row.h`: Lower-level operations on individual "rows" of the
  editor (a "row" corresponds to a line in the file we are editing)  
- `input.c`/`input.h`: Functions for getting input from the user.
- `screen.c`/`screen.h`: High-level functions for drawing and manipulating
  the editor's screen.
- `terminal.c`/`terminal.h`: Lower-level terminal operations.    
- `common.h`: Common definitions shared by multiple files.