
#include "ws_gencode.h"

    ws_header();

    ws_comment( "http://esolangs.org/wiki/Deadfish#Whitespace"
    "\n"	"by Robert de Bath (2015)");

    ws_push(0);	// x
ws_label(1);

    ws_push(62);
    ws_dup();
    ws_outc();
    ws_outc();
    ws_push(32);
    ws_outc();

    ws_dup();
    ws_push(256);
    ws_sub();
    ws_jz(2);

    ws_dup();
    ws_push(-1);
    ws_sub();
    ws_jz(2);
ws_label(3);

    ws_push(0);
    ws_readc();
    ws_push(0);
    ws_fetch();

    ws_dup();
    ws_push(105);	// i
    ws_sub();
    ws_jz(5);

    ws_dup();
    ws_push(100);	// d
    ws_sub();
    ws_jz(6);

    ws_dup();
    ws_push(115);	// s
    ws_sub();
    ws_jz(7);

    ws_dup();
    ws_push(111);	// o
    ws_sub();
    ws_jz(4);

    ws_drop();
    ws_push(10);
    ws_outc();
    ws_jump(1);

ws_label(2);
    ws_drop();
    ws_push(0);
    ws_jump(3);

ws_label(5);	// i
    ws_drop();
    ws_push(1);
    ws_add();
    ws_jump(1);

ws_label(6);	// d
    ws_drop();
    ws_push(-1);
    ws_add();
    ws_jump(1);

ws_label(7);	// 2
    ws_drop();
    ws_dup();
    ws_mul();
    ws_jump(1);

ws_label(4);	// o
    ws_drop();
    ws_dup();
    ws_outn();
    ws_push(10);
    ws_outc();
    ws_jump(1);

    ws_exit();

    ws_trailer
