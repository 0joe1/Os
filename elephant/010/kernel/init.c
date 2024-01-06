#include "interrupt.h"
#include "print.h"
#include "init.h"
void init()
{
    put_str("init all\n");
    init_all();
}
