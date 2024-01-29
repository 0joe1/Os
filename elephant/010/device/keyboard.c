#include "keyboard.h"
#include "print.h"

#define KDB_BUF_PORT 0x60

static void intr_keyboard_handler(void)
{
    uint_8 no_use;
    put_char('k');

}

void keyboard_init(void)
{
    put_str("keyboard init start\n");
    put_str("keyboard init done\n");
}

