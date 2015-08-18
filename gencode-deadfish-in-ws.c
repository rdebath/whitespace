
#include <stdio.h>

static void putnum(unsigned long num);
static void putsnum(long val);

static void ws_push(int v) { printf("  "); putsnum(v); }
static void ws_label(int v) { printf("\n  "); putnum(v); }
static void ws_jump(int v) { printf("\n \n"); putnum(v); }
static void ws_jz(int v) { printf("\n\t "); putnum(v); }

ws_dup() { printf(" \n "); }
ws_exit() { printf("\n\n\n"); }
ws_outc() { printf("\t\n  "); }
ws_readc() { printf("\t\n\t "); }

ws_add() { printf("\t   "); }
ws_mul() { printf("\t  \n"); }
ws_sub() { printf("\t  \t"); }

ws_drop() { printf(" \n\n"); }
ws_fetch() { printf("\t\t\t"); }
ws_outn() { printf("\t\n \t"); }
#define ws_retrieve ws_fetch

int
main(void)
{
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

    exit(0);
}





static void
putnum(unsigned long num)
{
    unsigned long v, max;

    max = 1; v = num;
    for(;;) {
	v /= 2;
	if (v == 0) break;
	max *= 2;
    }
#ifdef SEMIBUG
    putchar('#');
#endif
    for(;;) {
	v = num / max;
	num = num % max;
	if (v == 0) putchar(' '); else printf("\t");
	if (max == 1) break;
	max /= 2;
    }
    putchar('\n');
}

static void
putsnum(long val)
{
#ifdef SEMIBUG
    putchar('?');
#endif
    if (val >= 0) {
	putchar(' ');
	putnum(val);
    } else {
	printf("\t");
	putnum(-val);
    }
}

