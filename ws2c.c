
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

# ifndef NO_INTTYPES
#  include <inttypes.h>
#  define cell_t                intmax_t
#  define INTcell_C(mpm)        INTMAX_C(mpm)
#  define PRIdcell              PRIdMAX
#  define SCNdcell              SCNdMAX
# else
#  define cell_t                long
#  define INTcell_C(mpm)        mpm ## L
#  define PRIdcell              "ld"
#  define SCNdcell              "ld"
#  define uint32_t		unsigned /* I guess */
# endif

#define STFU(expression)	(expression)	/* Make GCC ... */

/* FNV Hash Constants from http://isthe.com/chongo/tech/comp/fnv/ */
#define HASH_FNV_INITIAL 2166136261U
#define HASH_FNV_MIXVAL 16777619U

/* For 64bit integers ...
    #define HASH_FNV_INITIAL 14695981039346656037UL
    #define HASH_FNV_MIXVAL  1099511628211UL
*/

#define HASH_MIX_FNV1A(hash, val) hash = (hash ^ (unsigned char)(val)) * HASH_FNV_MIXVAL

typedef uint32_t        fnv_hash_t;

/* Tune the bits if you wish. */
#define HASH_BITS       10
#define HASH_SIZE ((fnv_hash_t)1<<HASH_BITS)
#define HASH_MASK (HASH_SIZE-1)

#define TOKEN_LIST(Mac) \
    Mac(PUSH) \
    Mac(DUP) Mac(DROP) Mac(SWAP) \
    Mac(PICK) Mac(SLIDE) \
    Mac(LABEL) Mac(JUMP) Mac(CALL) \
    Mac(EXIT) \
    Mac(JZ) Mac(JN) \
    Mac(RETURN) \
    Mac(ADD) Mac(MUL) Mac(SUB) Mac(DIV) Mac(MOD) \
    Mac(OUTC) Mac(OUTN) Mac(READC) Mac(READN) \
    Mac(STORE) Mac(FETCH) \
    \
    Mac(NOP)

#define GEN_TOK_ENUM(NAME) T_ ## NAME,
enum token { TOKEN_LIST(GEN_TOK_ENUM) TCOUNT };

#define GEN_TOK_STRING(NAME) "T_" #NAME,
const char *tokennames[] = { TOKEN_LIST(GEN_TOK_STRING) };

char *cmdnames[TCOUNT];

struct pnode;

struct labelnode {
    struct labelnode *hashnext;
    struct labelnode *allnext;
    fnv_hash_t hash;
    struct pnode *location;
    struct pnode *reflist;
    char name[0];
};

struct pnode {
    struct pnode *next;
    struct pnode *prev;
    int type;
    cell_t value;
    int inum;

    struct labelnode *plabel;
    struct pnode *jmp;
    struct pnode *next_reflist;
};

#define EMBEDDED_WS
#include "ws_engine.h"
#include "ws_engine_txt.h"

cell_t cv_number(char *);
char *cv_label(char *);
char *cv_chr(char *);

void append_label(void);
void append_char(void);
void process_command(void);
void broken_command(void);
void init_cmdnames(void);

void process_token_v(int);
void process_token_i(int, cell_t);
void process_token_l(int, char *);
void process_tree(void);
void *tcalloc(size_t nmemb, size_t size);
struct pnode *add_node_after(struct pnode *p);
struct labelnode *find_label(char *label);
void run_tree(void);
void dump_tree(void);

struct pnode *wsprog = 0, *wsprogend;
struct labelnode *all_labels = 0;
struct labelnode *labeltab[HASH_SIZE];
#ifdef HASH_STATS
static long hash_num_labels, hash_num_searchs, hash_num_checks;
#endif

int inum = 0;

FILE *yyin;

char *yytext;
int yytext_size = 0, yytext_len;

char *yylabel;
int yylabel_size = 0, yylabel_len;

int opt_v0_2 = 0;
int interpret_now = 1;
int on_eof = -1;
int debug = 0;
int comment_lines = 0;

/* TODO:
    Header for GMP.
 */
int
main(int argc, char ** argv)
{
    int headerflg = 0;
    int enable_opts = 1;
    int done_file = 0;

    init_cmdnames();

    if (argc <= 1) yyin = stdin;

    do
    {
	if(argc>1 && argv[1][0] == '-' && enable_opts) {
	    if (!strcmp(argv[1], "--")) enable_opts = 1;
	    else if (!done_file && !strcmp(argv[1], "-r")) interpret_now = 1;
	    else if (!done_file && !strcmp(argv[1], "-c")) interpret_now = 0;
	    else if (!strcmp(argv[1], "-v")) comment_lines++;

	    else if (!strcmp(argv[1], "-e")) on_eof = -1;
	    else if (!strcmp(argv[1], "-z")) on_eof = 0;
	    else if (!strcmp(argv[1], "-n")) on_eof = 1;
	    else if (!strcmp(argv[1], "-d")) debug = 1;
	    else if (!strcmp(argv[1], "-old")) opt_v0_2 = 1;

	    else {
		fprintf(stderr, "Unknown option %s\n", argv[1]);
	    }
	    argv++, argc--;
	    continue;
	}

	if(argc>1) {
	    if (!(yyin = fopen(argv[1], "r"))) {
		perror(argv[1]);
		exit(1);
	    }
	    done_file = 1;
	}

	while (!feof(yyin) && !ferror(yyin)) {
	    if (!headerflg && !interpret_now) {
		printf("%s\n\nws_header();\n\n", header);
		headerflg = 1;
	    };
	    process_command();
	}

	if (ferror(yyin))
	    fprintf(stderr, "Error reading from file (ferror(3) so no reason given).\n");

	if(argc>1) {
	    fclose(yyin);
	    argv++, argc--;
	}
    } while(!done_file || argc>1);

    if (headerflg) printf("\nws_trailer\n");

    if (interpret_now) {
	setbuf(stdout, 0);
	process_tree();

	run_tree();
    }
#ifdef HASH_STATS
    if (hash_num_searchs) {
	printf("Hash stats:\n");
	printf("Number of searchs %ld\n", hash_num_searchs);
	printf("Number of search checks %ld\n", hash_num_checks);
	printf("Number of failed searchs %ld\n", hash_num_labels);
	printf("Average search length %f\n", (double)hash_num_checks/hash_num_searchs);
    }
#endif
    return 0;
}

void
process_command()
{
    yytext_len = 0;
    append_char();
    if (yytext_len == 0) return;

    append_char();
    append_char();

    if (feof(yyin)) {
	fprintf(stderr, "WARNING: Partial instruction at end of file: '%s'\n",
		 cv_chr(yytext));
	return;
    }

    if (yylabel_len != 0) {
	if (!interpret_now)
	    printf("/* %s*/\n", yylabel);
	else if (comment_lines > 0) {
	    comment_lines--;
	    printf("> %s\n", yylabel);
	}
    }
    yylabel_len = 0;

    if (yytext[0] == ' ') {
	if (yytext[1] == ' ') {
	    if (yytext[2] != '\n') {
		append_label();
		process_token_i(T_PUSH, cv_number(yytext+2));
	    } else
		broken_command();
	}
	if (yytext[1] == '\n') {
	    if (yytext[2] == ' ') process_token_v(T_DUP);
	    if (yytext[2] == '\n') process_token_v(T_DROP);
	    if (yytext[2] == '\t') process_token_v(T_SWAP);
	}
	if (yytext[1] == '\t') {
	    if (opt_v0_2)
		broken_command();
	    else if (yytext[2] != '\t') {
		append_label();
		if (yytext[2] == ' ') process_token_i(T_PICK, cv_number(yytext+3));
		if (yytext[2] == '\n') process_token_i(T_SLIDE, cv_number(yytext+3));
	    } else
		broken_command();
	}
    }

    if (yytext[0] == '\n') {
	if (yytext[1] == ' ') {
	    append_label();
	    if (yytext[2] == ' ') process_token_l(T_LABEL, cv_label(yytext+3));
	    if (yytext[2] == '\n') process_token_l(T_JUMP, cv_label(yytext+3));
	    if (yytext[2] == '\t') process_token_l(T_CALL, cv_label(yytext+3));
	}
	if (yytext[1] == '\n') {
	    if (yytext[2] == '\n')
		process_token_v(T_EXIT);
	    else
		broken_command();
	}
	if (yytext[1] == '\t') {
	    if (yytext[2] != '\n') {
		append_label();
		if (yytext[2] == ' ') process_token_l(T_JZ, cv_label(yytext+3));
		if (yytext[2] == '\t') process_token_l(T_JN, cv_label(yytext+3));
	    } else
		process_token_v(T_RETURN);
	}
    }

    if (yytext[0] == '\t') {
	if (yytext[1] == ' ') {
	    if (yytext[2] == '\n')
		broken_command();
	    else {
		append_char();
		if (yytext[2] == ' ') {
		    if (yytext[3] == ' ') process_token_v(T_ADD);
		    if (yytext[3] == '\n') process_token_v(T_MUL);
		    if (yytext[3] == '\t') process_token_v(T_SUB);
		}
		if (yytext[2] == '\t') {
		    if (yytext[3] == ' ') process_token_v(T_DIV);
		    if (yytext[3] == '\n') broken_command();
		    if (yytext[3] == '\t') process_token_v(T_MOD);
		}
	    }
	}
	if (yytext[1] == '\n') {
	    if (yytext[2] == '\n')
		broken_command();
	    else {
		append_char();
		if (yytext[2] == ' ') {
		    if (yytext[3] == ' ') process_token_v(T_OUTC);
		    if (yytext[3] == '\n') broken_command();
		    if (yytext[3] == '\t') process_token_v(T_OUTN);
		}
		if (yytext[2] == '\t') {
		    if (yytext[3] == ' ') process_token_v(T_READC);
		    if (yytext[3] == '\n') broken_command();
		    if (yytext[3] == '\t') process_token_v(T_READN);
		}
	    }
	}
	if (yytext[1] == '\t') {
	    if (yytext[2] == ' ') process_token_v(T_STORE);
	    if (yytext[2] == '\n') broken_command();
	    if (yytext[2] == '\t') process_token_v(T_FETCH);
	}
    }
}

void
broken_command()
{
    char *s = cv_chr(yytext);
    fprintf(stderr, "WARNING: Skipped unknown sequence: '%s'\n", s);
    if (!interpret_now) {
	printf("if (ws_%s) ws_%s();", s, s);
	printf("\t/* %s */\n", cv_chr(yytext));
    }
}

void
append_char()
{
    int ch;
    for(;;) {
	ch = getc(yyin);
	if (ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF) break;

	if ((ch > ' ' && ch <= '~') || ch >= 0x80 || ch < 0) {
	    if (ch == '/' && yylabel_len != 0 && yylabel[yylabel_len-1] == '*')
		ch = '\\'; /* Grrr */

	    if (yylabel_len+4 >= yylabel_size) {
		yylabel = realloc(yylabel, yylabel_size+=1024);
		if (yylabel == 0) { perror("malloc"); exit(99); }
	    }

	    yylabel[yylabel_len++] = ch;
	    yylabel[yylabel_len] = 0;
	}
    }
    if (ch == EOF) return;

    if (yytext_len+2 >= yytext_size) {
	yytext = realloc(yytext, yytext_size+=1024);
	if (yytext == 0) { perror("malloc"); exit(99); }
    }
    yytext[yytext_len++] = ch;
    yytext[yytext_len] = 0;

    if (yylabel_len > 0 && yylabel[yylabel_len-1] != ' ') {
	yylabel[yylabel_len++] = ' ';
	yylabel[yylabel_len] = 0;
    }
}

void
append_label()
{
    do {
	append_char();
    } while(yytext[yytext_len-1] != '\n' && !feof(yyin));
    if (feof(yyin))
	fprintf(stderr, "WARNING: Partial instruction at end of file: '%s'\n",
		 cv_chr(yytext));
}

cell_t
cv_number(char * ws_num)
{
    int negative = 0;
    cell_t value = 0;

    if (*ws_num) negative = (*ws_num++ != ' ');

    if (strlen(ws_num) > sizeof(cell_t) * 8)
	fprintf(stderr, "WARNING: Literal constant too large: '%s'\n", cv_chr(ws_num));
    if (*ws_num == '\n')
	fprintf(stderr, "WARNING: Literal constant is empty: '%s'\n", cv_chr(ws_num));

    while(*ws_num != '\n' && *ws_num) {
	value *= 2;
	if (*ws_num != ' ')
	    value++;
	ws_num++;
    }
    if (negative) value = -value;
    return value;
}

char * cv_label(char * ws_label)
{
    char * s;
    int l;
    static char * sbuf = 0;
    static int maxlen = 0;

    if ((l=strlen(ws_label)) > maxlen) {
	if(sbuf) free(sbuf);
	sbuf = malloc(l+8);
	maxlen = l;
    }

    if (*ws_label == '\0' || *ws_label == '\n')
	return "0x0";	/* Never return an empty string. */

    l--;

    if (ws_label[0] == ' ' && l%8 == 0 && l > 8) {
	int i = 0, j = 0, n = 0;
	for(s=ws_label; *s && *s != '\n'; s++) {
	    i *= 2;
	    if (*s != ' ')
		i ++;
	    if (++j == 8) {
		if (i>=48 && i<=57 && n != 0) {
		    sbuf[n++] = i;
		} else if ( (i>=65 && i<=90) || (i>=97 && i<=122) || i==95) {
		    sbuf[n++] = i;
		} else {
		    n = 0;
		    break;
		}
		i = j = 0;
	    }
	}
	if (n && *s && !s[1]) {
	    sbuf[n] = 0;
	    return sbuf;
	}
    }

    if (ws_label[0] != ' ' && l < 31) {
	int i = 0;
	for(s=ws_label; *s && *s != '\n'; s++) {
	    i *=2;
	    if (*s != ' ')
		i ++;
	}
	if (*s && !s[1]) {
	    sprintf(sbuf, "%d", i);
	    return sbuf;
	}
    }

    for (l=0; ws_label[l]; l++) switch(ws_label[l])
    {
	case ' ': sbuf[l] = '0'; break;
	case '\t': sbuf[l] = '1'; break;
	case '\n': sbuf[l] = '\0'; break;
	default: sbuf[l] = 'X'; break;
    }
    sbuf[l] = 0;
    return sbuf;
}

char * cv_chr(char * ws_code)
{
    int i;
    static char * sbuf = 0;
    static int maxlen = 0;

    if ((i=strlen(ws_code)) > maxlen) {
	if(sbuf) free(sbuf);
	sbuf = malloc(i+2);
	maxlen = i;
    }

    for (i=0; ws_code[i]; i++) switch(ws_code[i])
    {
	case ' ': sbuf[i] = 's'; break;
	case '\t': sbuf[i] = 't'; break;
	case '\n': sbuf[i] = 'l'; break;
	default: sbuf[i] = '*'; break;
	case '0': case '1': sbuf[i] = ws_code[i]; break;
    }
    sbuf[i] = 0;
    return sbuf;
}

void
init_cmdnames()
{
    /* No easy way to do this with the preprocessor */
    int i, j;
    for(i=0; i<TCOUNT; i++) {
	char * s = malloc(strlen(tokennames[i]));
	strcpy(s, "ws_");
	for(j=0; STFU(s[j] = tokennames[i][j+2]); j++)
	    s[j] = tolower(s[j]);
	cmdnames[i] = s;
    }
}

void
process_token_v(int token)
{
    if (!interpret_now) {
	inum ++;
#ifdef WS_TRACE
	printf("ws_trace(%d);\n", inum);
#endif
	printf("ws_%s();", cmdnames[token]);
	printf("\t/* %d: %s */\n", inum, cv_chr(yytext));
	return;
    } else {
	struct pnode * n = add_node_after(wsprogend);
	n->type = token;
	n->inum = ++inum;
    }
}

void
process_token_i(int token, cell_t value)
{
    if (!interpret_now) {
	inum ++;
#ifdef WS_TRACE
	printf("ws_trace(%d);\n", inum);
#endif
	printf("ws_%s(%"PRIdcell");", cmdnames[token], value);
	printf("\t/* %d: %s */\n", inum, cv_chr(yytext));
	return;
    } else {
	struct pnode * n = add_node_after(wsprogend);
	n->type = token;
	n->value = value;
	n->inum = ++inum;
    }
}

void
process_token_l(int token, char * label)
{
    if (!interpret_now) {
	inum ++;
#ifdef WS_TRACE
	if (token != T_LABEL) printf("ws_trace(%d);\n", inum);
#endif
	printf("ws_%s(%s);", cmdnames[token], label);
	printf("\t/* %d: %s */\n", inum, cv_chr(yytext));
	return;
    } else {
	struct pnode * n = add_node_after(wsprogend);
	n->type = token;
	n->plabel = find_label(label);
	n->inum = ++inum;
	if (token == T_LABEL) {
	    if (n->plabel->location) {
		fprintf(stderr, "WARNING: Label '%s' redefined, using first instance\n", label);
	    } else
		n->plabel->location = n;
	} else
	    n->jmp = n->plabel->location;

	n->next_reflist = n->plabel->reflist;
	n->plabel->reflist = n;
    }
}

void *
tcalloc(size_t nmemb, size_t size)
{
    void * m;
    m = calloc(nmemb, size);
    if (m) return m;

#if !defined(LEGACYOS) && !defined(_WIN32) && __STDC_VERSION__ >= 199901L
    fprintf(stderr, "Allocate of %zu*%zu bytes failed, ABORT\n", nmemb, size);
#else
    fprintf(stderr, "Allocate of %lu*%lu bytes failed, ABORT\n",
	    (unsigned long)nmemb, (unsigned long)size);
#endif
    exit(42);
}

struct pnode *
add_node_after(struct pnode * p)
{
    struct pnode * n = tcalloc(1, sizeof*n);
    n->type = T_NOP;
    if (p) {
	n->prev = p;
	n->next = p->next;
	if (n->next) n->next->prev = n;
	n->prev->next = n;
    } else if (wsprog) {
	n->next = wsprog;
	if (n->next) n->next->prev = n;
    } else wsprog = n;

    if (p == wsprogend) wsprogend = n;
    return n;
}

static fnv_hash_t
hash_string(const char *val)
{
    fnv_hash_t hash = HASH_FNV_INITIAL;
    unsigned char c;

    while ((c = (unsigned char)*val++))
	HASH_MIX_FNV1A(hash, c);

    /* Humm, I don't see the point of these two variations, they give
     * identical results given the preconditions. */
#if HASH_BITS < 16
    hash = (((hash>>HASH_BITS) ^ hash) & HASH_MASK);
#else
    hash = (hash>>HASH_BITS) ^ (hash & HASH_MASK);
#endif
    return hash;
}

struct labelnode *
find_label(char *label)
{
    fnv_hash_t hash = hash_string(label);
    struct labelnode *n, *p;

#ifdef HASH_STATS
    hash_num_searchs++;
#endif
    for(p = 0, n = labeltab[hash]; n; p = n, n = n->hashnext) {
#ifdef HASH_STATS
	hash_num_checks++;
#endif
	if (n->hash == hash && strcmp(n->name, label) == 0)
	    return n;
    }

#ifdef HASH_STATS
    hash_num_labels++;
#endif
    n = tcalloc(1, sizeof *n + strlen(label) + 1);
    strcpy(n->name, label);
    n->hash = hash;
    if (p)
	p->hashnext = n;
    else
	labeltab[hash] = n;

    if (all_labels) n->allnext = all_labels;
    all_labels = n;

    return n;
}

void
process_tree()
{
    struct pnode *n;
    struct labelnode * l;

    /* Falling off the end is a *really* bad idea */
    if (!wsprogend || wsprogend->type != T_EXIT) {
	n = add_node_after(wsprogend);
	n->type = T_EXIT;
	n->inum = ++inum;
    }

    /* Point all the labels at the instruction after the label command */
    for(l = all_labels; l; l=l->allnext)
    {
	if (l->location == 0) {
	    fprintf(stderr, "WARNING: Label '%s' not defined, mapping to exit()\n", l->name);
	    l->location = wsprogend;
	}
	while (l->location->type == T_LABEL)
	    l->location = l->location->next;

	for(n = l->reflist; n; n=n->next_reflist) {
	    n->jmp = l->location;
	}
    }

    /* Snip out all the label commands and clear the label pointers. */
    for(n = wsprog; n; n=n->next) {
	if (n->type == T_LABEL) {
	    struct pnode *p = n->prev;
	    if (p) p->next = n->next; else wsprog = n->next;
	    if (n->next) n->next->prev = n->prev; else wsprogend = n->prev;
	    free(n);
	    n = p;
	    if (!n) n = wsprog;
	}
	n->plabel = 0;
    }

    /* Set the label pointers for instructions that are pointed at by labels. */
    for(l = all_labels; l; l=l->allnext)
    {
	l->location->plabel = l;
    }
}

void
dump_tree()
{
    struct pnode *n, *p;

    for(n = wsprog; n; n=n->next) {
	if (n->plabel)
	    printf("ws_label(%s);\n", n->plabel->name);

	printf("ws_%s", cmdnames[n->type]);
	if (n->type == T_PUSH || n->type == T_PICK || n->type == T_SLIDE)
	    printf("(%"PRIdcell");", n->value);
	else if (n->jmp)
	    printf("(%s);", n->jmp->plabel->name);
	else
	    printf("();");

	printf("\t/*%4d", n->inum);

	if (n->jmp)
	    printf(" --> %d", n->jmp->inum);

	if (n->plabel) {
	    int c = ' ';
	    printf(" <--");
	    for(p = n->plabel->reflist; p; p=p->next_reflist) {
		if (p->type != T_LABEL) {
		    printf("%c%d", c, p->inum);
		    c = ',';
		}
	    }
	    if (c == ' ') printf(" UNUSED");
	}

	printf(" */");
	printf("\n");
    }
}

void
run_tree()
{
    struct pnode * n;

    for(n = wsprog; n; ) {
#ifdef WS_TRACE
	ws_trace(n->inum);
#endif
	switch(n->type) {
	case T_PUSH:
	    ws_push(n->value);
	    break;

	case T_DUP:
	    ws_dup();
	    break;

	case T_DROP:
	    ws_drop();
	    break;

	case T_SWAP:
	    ws_swap();
	    break;

	case T_PICK:
	    ws_pick(n->value);
	    break;

	case T_SLIDE:
	    ws_slide(n->value);
	    break;

	case T_JUMP:
	    n = n->jmp;
	    continue;

	case T_CALL:
	    rpush(n);
	    n = n->jmp;
	    continue;

	case T_EXIT:
	    return;

	case T_JZ:
	    if(ws_pop() == 0) {
		n = n->jmp;
		continue;
	    }
	    break;

	case T_JN:
	    if(ws_pop() < 0) {
		n = n->jmp;
		continue;
	    }
	    break;

	case T_RETURN:
	    n = rpop();
	    break;

	case T_ADD:
	    ws_add();
	    break;

	case T_MUL:
	    ws_mul();
	    break;

	case T_SUB:
	    ws_sub();
	    break;

	case T_DIV:
	    ws_div();
	    break;

	case T_MOD:
	    ws_mod();
	    break;

	case T_OUTC:
	    ws_outc();
	    break;

	case T_OUTN:
	    ws_outn();
	    break;

	case T_READC:
	    ws_readc();
	    break;

	case T_READN:
	    ws_readn();
	    break;

	case T_STORE:
	    ws_store();
	    break;

	case T_FETCH:
	    ws_fetch();
	    break;

	default:
	    fprintf(stderr, "%s unimplemented\n", tokennames[n->type]); exit(1);
	    break;
	}

	n=n->next;
    }
}
