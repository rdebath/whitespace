
#ifndef EMBEDDED_WS

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# ifndef NO_INTTYPES
#  include <inttypes.h>
#  define cell_t		intmax_t
#  define INTcell_C(mpm)	INTMAX_C(mpm)
#  define PRIdcell		PRIdMAX
#  define SCNdcell		SCNdMAX
# else
#  define cell_t		long
#  define INTcell_C(mpm)	mpm ## L
#  define PRIdcell		"ld"
#  define SCNdcell		"ld"
# endif

typedef int rtnstr_t;

# define ws_label(x)	L_ ## x:
# define ws_call(x)	rpush(__LINE__); goto L_ ## x; case __LINE__:
# define ws_jump(x)	goto L_ ## x
# define ws_jz(x)	if(ws_pop() == 0) goto L_ ## x
# define ws_jn(x)	if(ws_pop() < 0) goto L_ ## x
# define ws_return()	continue
# define ws_exit()	exit(0)

#else
typedef struct pnode * rtnstr_t;
#endif

#ifdef __GNUC__
# define GCC_UNUSED __attribute__ ((unused))
#else
# define GCC_UNUSED
#endif

#define LOCALLIB static inline GCC_UNUSED

#define LOCALLIB static inline GCC_UNUSED

cell_t *main_stack = 0;
int main_slen = 0, main_sp = 0;

rtnstr_t * retr_stack = 0;
int retr_slen = 0, retr_sp = 0;

cell_t *memory = 0;
int memlen = 0;

#ifndef NO_MEMORYCHECK
static void check_memory(cell_t mptr) {
static int mccount = 5;
    if (mptr >= 0 && mptr < memlen) return;
#ifdef EMBEDDED_WS
    if (enable_warnings == 0) return;
#endif
    if (mccount == 0) return;

    if (mptr < 0) {
	fprintf(stderr, "Memory pointer %"PRIdcell" is illegal.\n", mptr);
    } else {
	fprintf(stderr, "Memory pointer %"PRIdcell" has not been allocated.\n", mptr);
    }

    if (--mccount == 0)
	fprintf(stderr, "Disabling memory_check() for this run\n");
}
#endif

static void extend_memory(cell_t mptr) {
    int oldmsize, newmsize, i;
    if (mptr < 0) {
	fprintf(stderr, "Memory pointer %"PRIdcell" is illegal.\n", mptr);
	exit(99);
    }
    if (mptr < memlen) return;

    oldmsize = ((memlen+4095)/4096)*4096;
    memlen = mptr + 1;
    newmsize = ((memlen+4095)/4096)*4096;
    if (oldmsize == newmsize) return;
    memory = realloc(memory, newmsize*sizeof*memory);
    if (memory == 0) {
	fprintf(stderr, "Out of memory: %"PRIdcell"\n", mptr);
	exit(4);
    }
    for(i=oldmsize; i<newmsize; i++)
	memory[i] = 0;
}

static void rextend(void) {
    retr_stack = realloc(retr_stack, (retr_slen += 1024)*sizeof*retr_stack);
    if (!retr_stack) {
	perror("wsi: return stack");
	exit(1);
    }
}

LOCALLIB void rpush(rtnstr_t val) {
    if (retr_sp >= retr_slen) rextend();
    retr_stack[retr_sp++] = val;
}

LOCALLIB rtnstr_t rpop() {
    if(retr_sp == 0) {
	fprintf(stderr, "Return stack underflow\n");
	exit(99);
    }
    return retr_stack[--retr_sp];
}

static void mstackempty(void) {
    fprintf(stderr, "Main stack underflow\n");
    exit(99);
}

static void mextend(void) {
    main_stack = realloc(main_stack, (main_slen += 1024)*sizeof*main_stack);
    if (!main_stack) {
	perror("wsi: main stack");
	exit(1);
    }
}

#ifdef NO_STACKCHECK
# define mstacktwo()
# define mstackone()
#else
LOCALLIB void mstacktwo() { if (main_sp < 2) mstackempty(); }
LOCALLIB void mstackone() { if (main_sp < 1) mstackempty(); }
#endif

#define TOS (main_stack[main_sp-1])
#define NOS (main_stack[main_sp-2])

LOCALLIB void ws_push(cell_t val) {
    if (main_sp >= main_slen) mextend();
    main_stack[main_sp++] = val;
}

LOCALLIB cell_t ws_pop() {
    mstackone();
    return main_stack[--main_sp];
}

LOCALLIB void ws_drop() { ws_pop(); }
LOCALLIB void ws_dup() { mstackone(); ws_push(TOS); }
LOCALLIB void ws_swap() { cell_t t; mstacktwo(); t = TOS; TOS = NOS; NOS = t; }
LOCALLIB void ws_add() { mstacktwo(); NOS = NOS + TOS; main_sp--; }
LOCALLIB void ws_sub() { mstacktwo(); NOS = NOS - TOS; main_sp--; }
LOCALLIB void ws_mul() { mstacktwo(); NOS = NOS * TOS; main_sp--; }
LOCALLIB void ws_div() { mstacktwo(); if (TOS) NOS = NOS / TOS; else NOS=0; main_sp--; }
LOCALLIB void ws_mod() { mstacktwo(); if (TOS) NOS = NOS % TOS; else NOS=0; main_sp--; }

LOCALLIB void ws_store() {
    mstacktwo();
    extend_memory(NOS);
    memory[NOS] = TOS;
    main_sp -= 2;
}

LOCALLIB void ws_fetch() {
    mstackone();
    if (TOS < 0 || TOS >= memlen) {
#ifndef NO_MEMORYCHECK
	check_memory(TOS);
#endif
	TOS = 0;
	return;
    }
    TOS = memory[TOS];
}

LOCALLIB void ws_outc() { mstackone(); putchar(TOS); main_sp--; }

LOCALLIB void ws_readc() {
    mstackone();
    extend_memory(TOS);
    memory[TOS] = getchar();
    main_sp--;
}

static GCC_UNUSED void ws_outn() { mstackone(); printf("%"PRIdcell, TOS); main_sp--; }

static GCC_UNUSED void ws_readn() {
    char ibuf[1024];
    mstackone();
    extend_memory(TOS);
    if (fgets(ibuf, sizeof(ibuf), stdin)) {
	cell_t val = 0;
	sscanf(ibuf, "%"SCNdcell, &val);
	memory[TOS] = val;
    } else
	memory[TOS] = 0;
    main_sp--;
}

static GCC_UNUSED void ws_pick(cell_t val) {
    if(val < 0 || val >= main_sp) mstackempty();
    ws_push(main_stack[main_sp-val-1]);
}

static GCC_UNUSED void ws_slide(cell_t val) {
    cell_t t;
    mstackone();
    t = TOS;
    while(val-->0 && main_sp) ws_drop();
    mstackone();
    TOS = t;
}

#ifdef WS_TRACE
static GCC_UNUSED void ws_trace(int val) {
    fprintf(stderr, "I %d %d %d %d\n",
	val,
	main_sp,
	main_sp>0?(int)TOS:0,
	main_sp>1?(int)NOS:0
	);
}
#endif

#define ws_comment(x)

#define ws_header()		\
int				\
main(void)			\
{				\
    setbuf(stdout, 0);		\
    rpush(-1);			\
				\
    while(1) {			\
	switch(rpop()) {	\
	case -1:

#define ws_trailer } ws_exit(); } }

/****************************************************************************/
