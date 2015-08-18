
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifndef BIJECTIVE
#define BIJECTIVE 0
#endif

static void putnum(unsigned long num, int clipped);
static void putsnum(long val);
static int atom(char * s);

static void ws_push(int v) { printf("  "); putsnum(v); }
static void ws_label(int v) { printf("\n  "); putnum(v, BIJECTIVE); }
static void ws_jump(int v) { printf("\n \n"); putnum(v, BIJECTIVE); }
static void ws_jz(int v) { printf("\n\t "); putnum(v, BIJECTIVE); }
static void ws_jn(int v) { printf("\n\t\t"); putnum(v, BIJECTIVE); }
static void ws_call(int v) { printf("\n \t"); putnum(v, BIJECTIVE); }
static void ws_exit() { printf("\n\n\n"); }
static void ws_return() { printf("\n\t\n"); }

static void ws_dup() { printf(" \n "); }
static void ws_swap() { printf(" \n\t"); }
static void ws_drop() { printf(" \n\n"); }

static void ws_outc() { printf("\t\n  "); }
static void ws_readc() { printf("\t\n\t "); }
static void ws_outn() { printf("\t\n \t"); }
static void ws_readn() { printf("\t\n\t\t"); }

static void ws_add() { printf("\t   "); }
static void ws_sub() { printf("\t  \t"); }
static void ws_mul() { printf("\t  \n"); }
static void ws_div() { printf("\t \t "); }
static void ws_mod() { printf("\t \t\t"); }

static void ws_fetch() { printf("\t\t\t"); }
static void ws_store() { printf("\t\t "); }

#define ws_retrieve ws_fetch

/* Version 0.3 functions */
static void ws_pick(int v) { printf(" \t "); putsnum(v); }
static void ws_slide(int v) { printf(" \t\n"); putsnum(v); }

/* Minor macros */

/* Push a string onto the stack in reverse order. */
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

/* Jump if positive */
static void ws_jp(int v) {
    int t = atom("");
    ws_dup();
    ws_jn(t);
    ws_dup();
    ws_jz(t);
    ws_drop();
    ws_jump(v);
    ws_label(t);
    ws_drop();
}

/* Jump if positive or zero */
static void ws_jpz(int v) {
    int t = atom("");
    ws_jn(t);
    ws_jump(v);
    ws_label(t);
}

/* Jump if negative or zero */
static void ws_jnz(int v) {
    int t1 = atom("");
    int t2 = atom("");
    ws_dup();
    ws_jn(t1);
    ws_dup();
    ws_jz(t1);
    ws_jump(t2);
    ws_label(t1);
    ws_drop();
    ws_jump(v);
    ws_label(t2);
    ws_drop();
}

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

/*
 * This is VERY slow, if you have more than a few dozen symbols RELACE THIS.
 *
 * Note: an empty string gives a unique id.
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
    if(*s) for(i=0; i<curcount; i++) {
	if (strcmp(s, stringlist[i]) == 0)
	    return i+1;
    }
    stringlist[curcount++] = s;
    // fprintf(stderr, "Label %d = \"%s\"\n", curcount, s);
    return curcount;
}
