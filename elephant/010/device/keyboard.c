#include "keyboard.h"
#include "print.h"
#include "interrupt.h"
#include "io.h"

#define KDB_BUF_PORT 0x60

static void intr_keyboard_handler(void)
{
    uint_8 scancode;
    scancode = inb(KDB_BUF_PORT);
    put_int(scancode);put_char(' ');
}

void keyboard_init(void)
{
    put_str("keyboard init start\n");
    register_handler(0x21,intr_keyboard_handler);
    put_str("keyboard init done\n");
}

