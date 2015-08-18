
#include "ws_gencode.h"

int
main(void)
{
    ws_exit();

    ws_label(0);
    ws_dup();
    ws_jz(1);
    ws_outc();
    ws_jump(0);

    ws_label(1);
    ws_drop();
    ws_return();

    ws_exit();

    exit(0);
}
