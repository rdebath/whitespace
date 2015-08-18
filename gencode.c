
#include <stdio.h>

static void putnum(unsigned long num);
static void putsnum(long val);

static void ws_push(int v) { printf("  "); putsnum(v); }
static void ws_label(int v) { printf("\n  "); putnum(v); }
static void ws_jump(int v) { printf("\n \n"); putnum(v); }
static void ws_jz(int v) { printf("\n\t "); putnum(v); }
static void ws_call(int v) { printf("\n \t"); putnum(v); }

static void ws_dup() { printf(" \n "); }
static void ws_exit() { printf("\n\n\n"); }
static void ws_outc() { printf("\t\n  "); }
static void ws_readc() { printf("\t\n\t "); }

static void ws_add() { printf("\t   "); }
static void ws_mul() { printf("\t  \n"); }
static void ws_sub() { printf("\t  \t"); }

static void ws_drop() { printf(" \n\n"); }
static void ws_fetch() { printf("\t\t\t"); }
static void ws_outn() { printf("\t\n \t"); }
#define ws_retrieve ws_fetch

static void ws_return() { printf("\n\t\n"); }

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

