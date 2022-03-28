#include "terminal.h"
#include "editor.h"
#include "screen.h"
#include "input.h"

int main(int argc, char *argv[])
{
    editor_ctx_t ctx;

    terminal_enable_raw_mode();
    init_editor(&ctx);

    if (argc >= 2)
    {
        editor_open_file(&ctx, argv[1]);
    }

    screen_set_status_message(&ctx, "HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

    while (1)
    {
        screen_refresh(&ctx);
        input_process_keypress(&ctx);
    }

    return 0;
}