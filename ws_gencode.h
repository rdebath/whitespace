
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifndef BIJECTIVE
#define BIJECTIVE 0
#endif

static void putnum(unsigned long num, int clipped);
static void putsnum(long val);

static void ws_push(int v) { printf("  "); putsnum(v); }
static void ws_label(int v) { printf("\n  "); putnum(v, BIJECTIVE); }
static void ws_jump(int v) { printf("\n \n"); putnum(v, BIJECTIVE); }
static void ws_jz(int v) { printf("\n\t "); putnum(v, BIJECTIVE); }
static void ws_jn(int v) { printf("\n\t\t"); putnum(v, BIJECTIVE); }
static void ws_call(int v) { printf("\n \t"); putnum(v, BIJECTIVE); }

static void ws_dup() { printf(" \n "); }
static void ws_swap() { printf(" \n\t"); }
static void ws_exit() { printf("\n\n\n"); }

static void ws_outc() { printf("\t\n  "); }
static void ws_readc() { printf("\t\n\t "); }

static void ws_add() { printf("\t   "); }
static void ws_mul() { printf("\t  \n"); }
static void ws_sub() { printf("\t  \t"); }

static void ws_drop() { printf(" \n\n"); }
static void ws_fetch() { printf("\t\t\t"); }
static void ws_store() { printf("\t\t "); }

static void ws_outn() { printf("\t\n \t"); }
#define ws_retrieve ws_fetch

static void ws_return() { printf("\n\t\n"); }


static void
putnum(unsigned long num, int clipped)
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
    if (clipped) { num -= max; max /= 2; }
    if (max) for(;;) {
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
	putnum(val, 0);
    } else {
	printf("\t");
	putnum(-val, 0);
    }
}

static void ws_pushs(char * s)
{
    int c=0, i = strlen(s) +1;
    for(; i>0 ; i--) {
	if (c!=0 && c == s[i-1])
	    ws_dup();
	else
	    ws_push(c = s[i-1]);
    }
}

/*
 * This is VERY slow, if you have more than a few dozen symbols RELACE THIS.
 */
static int atom(char * s)
{
    static char ** stringlist;
    static int maxcount = 0;
    static int curcount = 0;
    int i;

    if (curcount >= maxcount) {
	stringlist = realloc(stringlist, (maxcount+=1024)*sizeof*stringlist);
    }
    for(i=0; i<curcount; i++) {
	if (strcmp(s, stringlist[i]) == 0)
	    return i+1;
    }
    stringlist[curcount++] = s;
    return curcount;
}
