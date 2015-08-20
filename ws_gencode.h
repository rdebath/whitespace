
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifndef BIJECTIVE
#define BIJECTIVE 0
#endif

#ifdef ATOMISE_LABELS

#define ws_label(x) ws_label_int(atom(#x))
#define ws_jump(x) ws_jump_int(atom(#x))
#define ws_jz(x) ws_jz_int(atom(#x))
#define ws_jn(x) ws_jn_int(atom(#x))
#define ws_jp(x) ws_jp_int(atom(#x))
#define ws_jzn(x) ws_jzn_int(atom(#x))
#define ws_jzp(x) ws_jzp_int(atom(#x))
#define ws_jnz(x) ws_jnz_int(atom(#x))
#define ws_call(x) ws_call_int(atom(#x))

#else

#define ws_label(x) ws_label_int(x)
#define ws_jump(x) ws_jump_int(x)
#define ws_jz(x) ws_jz_int(x)
#define ws_jn(x) ws_jn_int(x)
#define ws_jp(x) ws_jp_int(x)
#define ws_jzn(x) ws_jzn_int(x)
#define ws_jzp(x) ws_jzp_int(x)
#define ws_jnz(x) ws_jnz_int(x)
#define ws_call(x) ws_call_int(x)

#endif

static void putnum(unsigned long num, int clipped);
static void putsnum(long val);
static int atom(char * s);

static void ws_label_int(int v) { printf("\n  "); putnum(v, BIJECTIVE); }
static void ws_jump_int(int v) { printf("\n \n"); putnum(v, BIJECTIVE); }
static void ws_jz_int(int v) { printf("\n\t "); putnum(v, BIJECTIVE); }
static void ws_jn_int(int v) { printf("\n\t\t"); putnum(v, BIJECTIVE); }
static void ws_call_int(int v) { printf("\n \t"); putnum(v, BIJECTIVE); }

static void ws_push(int v) { printf("  "); putsnum(v); }
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
    int c=0, i;
#ifdef VERBOSE
    printf("pushs(\"");
    for(i=0; s[i]; i++)
	if ( (s[i] >= '!' && s[i] <= '~') && s[i] != '\\' && s[i] != '"')
	    putchar(s[i]);
	else
	    printf("\\%03o", (unsigned char) s[i]);
    printf("\")");
#endif
    for(i = strlen(s) +1; i>0 ; i--) {
	if (c!=0 && c == s[i-1])
	    ws_dup();
	else
	    ws_push(c = s[i-1]);
    }
}

#ifdef ATOMISE_LABELS
/* These only work properly if they can allocate a unique atom */

/* Jump if positive */
static void ws_jp_int(int v) {
    int t = atom("");
    ws_dup();
    ws_jn_int(t);
    ws_dup();
    ws_jz_int(t);
    ws_drop();
    ws_jump_int(v);
    ws_label_int(t);
    ws_drop();
}

/* Jump if zero or positive */
static void ws_jzp_int(int v) {
    int t = atom("");
    ws_jn_int(t);
    ws_jump_int(v);
    ws_label_int(t);
}

/* Jump if zero or negative */
static void ws_jzn_int(int v) {
    int t1 = atom("");
    int t2 = atom("");
    ws_dup();
    ws_jn_int(t1);
    ws_dup();
    ws_jz_int(t1);
    ws_jump_int(t2);
    ws_label_int(t1);
    ws_drop();
    ws_jump_int(v);
    ws_label_int(t2);
    ws_drop();
}

/* Jump if Not zero */
static void ws_jnz_int(int v) {
    int t = atom("");
    ws_jz_int(t);
    ws_jump_int(v);
    ws_label_int(t);
}

#endif

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
 * Note: Returned IDs start at one.
 * Note: An empty string gives a unique id.
 */
static char ** atom_list;
static int atom_size = 0;
static int atom_len = 0;

static int atom(char * s)
{
    int i;
#ifdef VERBOSE
    for(i=0; s[i]; i++)
	if ( (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'Z') ||
	     (s[i] >= 'a' && s[i] <= 'z') || s[i] == '_')
	    putchar(s[i]);
#endif

    if (atom_len >= atom_size) {
	atom_list = realloc(atom_list, (atom_size+=1024)*sizeof*atom_list);
    }
    if(*s) for(i=0; i<atom_len; i++) {
	if (strcmp(s, atom_list[i]) == 0)
	    return i+1;
    }
    atom_list[atom_len++] = s;
    return atom_len;
}

static void
print_atom_list()
{
    int i;

    for(i=0; i<atom_len; i++) {
	printf("Label %d = \"%s\"\n", i+1, atom_list[i]);
    }
}

static void
ws_comment(char * msg)
{
    int nl = 0;
    int col = 0;
    char * s;

    for (s = msg; *s; s++) {
	if (col == 0) {
	    printf("   \t");
	    col ++;
	}
	if ( (*s >= ' ' && *s <= '~') || *s == '\t')
	    putchar(*s);
	else if (*s == '\n') {
	    putchar(*s);
	    col = 0;
	    nl ++;
	}
    }
    if (col) { putchar('\n'); nl++; }
    while(nl-->1) ws_add();
    ws_drop();
}
