
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
# endif

#define STFU(expression)	(expression)	/* Make GCC ... */

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
    struct labelnode *next;
    struct pnode *location;
    struct pnode *reflist;
    char *name;
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
void dump_tree(void);

struct pnode *wsprog = 0, *wsprogend;
struct labelnode *labellist = 0;
int inum = 0;

FILE *yyin;

char *yytext;
int yytext_size = 0, yytext_len;

char *yylabel;
int yylabel_size = 0, yylabel_len;

char *header;

int opt_v0_2 = 0;
int interpret_now = 0;
int on_eof = -1;
int debug = 0;

/* TODO:
    Add fallbacks for labels that are not defined.
    Run C code via DLL.
    Header for GMP.
    Built in interpreter.
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
	    if (!headerflg && !interpret_now) { printf("%s\n\n", header); headerflg = 1; };
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

	dump_tree();
    }
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

    if (yylabel_len != 0 && !interpret_now)
	printf("/* %s*/\n", yylabel);
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
    int negative = (*ws_num++ != ' ');
    cell_t value = 0;

    if (strlen(ws_num) > sizeof(cell_t) * 8)
	fprintf(stderr, "WARNING: Literal constant too large: '%s'\n", cv_chr(ws_num));
    if (*ws_num == '\n')
	fprintf(stderr, "WARNING: Literal constant is empty: '%s'\n", cv_chr(ws_num));

    while(*ws_num != '\n') {
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
	printf("ws_%s();", cmdnames[token]);
	printf("\t/* %d: %s */\n", ++inum, cv_chr(yytext));
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
	printf("ws_%s(%"PRIdcell");", cmdnames[token], value);
	printf("\t/* %d: %s */\n", ++inum, cv_chr(yytext));
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
	printf("ws_%s(%s);", cmdnames[token], label);
	printf("\t/* %d: %s */\n", ++inum, cv_chr(yytext));
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

struct labelnode *
find_label(char *label)
{
    /*
     * BEWARE: Linear search: TODO: replace!!
     */
    struct labelnode *n, *p;
    for(p = 0, n = labellist; n; p = n, n = n->next) {
	if (strcmp(n->name, label) == 0)
	    return n;
    }

    n = tcalloc(1, sizeof *n);
    n->name = strdup(label);
    if (p)
	p->next = n;
    else
	labellist = n;

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
    for(l = labellist; l; l=l->next)
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
    for(l = labellist; l; l=l->next)
    {
	l->location->plabel = l;
    }
}

void
dump_tree()
{
    struct pnode *n, *p;

    for(n = wsprog; n; n=n->next) {
	if (n->plabel) {
	    printf("L_%s: ", n->plabel->name);
	}

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

char * header =

	"#include <stdio.h>"
"\n"	"#include <stdlib.h>"
"\n"	"#include <unistd.h>"
"\n"
"\n"	"# ifndef NO_INTTYPES"
"\n"	"#  include <inttypes.h>"
"\n"	"#  define cell_t\t\tintmax_t"
"\n"	"#  define INTcell_C(mpm)\tINTMAX_C(mpm)"
"\n"	"#  define PRIdcell\t\tPRIdMAX"
"\n"	"#  define SCNdcell\t\tSCNdMAX"
"\n"	"# else"
"\n"	"#  define cell_t\t\tlong"
"\n"	"#  define INTcell_C(mpm)\tmpm ## L"
"\n"	"#  define PRIdcell\t\t\"ld\""
"\n"	"#  define SCNdcell\t\t\"ld\""
"\n"	"# endif"
"\n"
"\n"	"# ifdef __GNUC__"
"\n"	"#  define GCC_UNUSED __attribute__ ((unused))"
"\n"	"# else"
"\n"	"#  define GCC_UNUSED"
"\n"	"# endif"
"\n"
"\n"	"#define LOCALLIB static inline GCC_UNUSED"
"\n"
"\n"	"#define ws_label(x)\tL_ ## x:"
"\n"	"#define ws_call(x)\trpush(__LINE__); goto L_ ## x; case __LINE__:"
"\n"	"#define ws_jump(x)\tgoto L_ ## x"
"\n"	"#define ws_jz(x)\tif(ws_pop() == 0) goto L_ ## x"
"\n"	"#define ws_jn(x)\tif(ws_pop() < 0) goto L_ ## x"
"\n"	"#define ws_return()\tcontinue"
"\n"	"#define ws_exit()\texit(0)"
"\n"
"\n"	"cell_t * main_stack = 0;"
"\n"	"int main_slen = 0, main_sp = 0;"
"\n"
"\n"	"int * retr_stack = 0;"
"\n"	"int retr_slen = 0, retr_sp = 0;"
"\n"
"\n"	"cell_t *memory = 0;"
"\n"	"int memlen = 0;"
"\n"
"\n"	"# ifndef NO_MEMORYCHECK"
"\n"	"static void check_memory(cell_t mptr) {"
"\n"	"static int mccount = 5;"
"\n"	"    if (mptr >= 0 && mptr < memlen) return;"
"\n"	"    if (mccount == 0) return;"
"\n"
"\n"	"    if (mptr < 0) {"
"\n"	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" is illegal.\\n\", mptr);"
"\n"	"    } else {"
"\n"	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" has not been allocated.\\n\", mptr);"
"\n"	"    }"
"\n"
"\n"	"    if (--mccount == 0)"
"\n"	"\tfprintf(stderr, \"Disabling memory_check() for this run\\n\");"
"\n"	"}"
"\n"	"# endif"
"\n"
"\n"	"static void extend_memory(cell_t mptr) {"
"\n"	"    int oldmsize, newmsize, i;"
"\n"	"    if (mptr < 0) {"
"\n"	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" is illegal.\\n\", mptr);"
"\n"	"\texit(99);"
"\n"	"    }"
"\n"	"    if (mptr < memlen) return;"
"\n"
"\n"	"    oldmsize = ((memlen+4095)/4096)*4096;"
"\n"	"    memlen = mptr + 1;"
"\n"	"    newmsize = ((memlen+4095)/4096)*4096;"
"\n"	"    if (oldmsize == newmsize) return;"
"\n"	"    memory = realloc(memory, newmsize*sizeof*memory);"
"\n"	"    if (memory == 0) {"
"\n"	"\tfprintf(stderr, \"Out of memory: %\"PRIdcell\"\\n\", mptr);"
"\n"	"\texit(4);"
"\n"	"    }"
"\n"	"    for(i=oldmsize; i<newmsize; i++)"
"\n"	"\tmemory[i] = 0;"
"\n"	"}"
"\n"
"\n"	"static void rextend(void) {"
"\n"	"    retr_stack = realloc(retr_stack, (retr_slen += 1024)*sizeof*retr_stack);"
"\n"	"    if (!retr_stack) {"
"\n"	"\tperror(\"wsi: return stack\");"
"\n"	"\texit(1);"
"\n"	"    }"
"\n"	"}"
"\n"
"\n"	"LOCALLIB void rpush(int val) {"
"\n"	"    if (retr_sp >= retr_slen) rextend();"
"\n"	"    retr_stack[retr_sp++] = val;"
"\n"	"}"
"\n"
"\n"	"LOCALLIB int rpop() {"
"\n"	"    if(retr_sp == 0) {"
"\n"	"\tfprintf(stderr, \"Return stack underflow\\n\");"
"\n"	"\texit(99);"
"\n"	"    }"
"\n"	"    return retr_stack[--retr_sp];"
"\n"	"}"
"\n"
"\n"	"static void mstackempty(void) {"
"\n"	"    fprintf(stderr, \"Main stack underflow\\n\");"
"\n"	"    exit(99);"
"\n"	"}"
"\n"
"\n"	"static void mextend(void) {"
"\n"	"    main_stack = realloc(main_stack, (main_slen += 1024)*sizeof*main_stack);"
"\n"	"    if (!main_stack) {"
"\n"	"\tperror(\"wsi: main stack\");"
"\n"	"\texit(1);"
"\n"	"    }"
"\n"	"}"
"\n"
"\n"	"# ifdef NO_STACKCHECK"
"\n"	"#  define mstacktwo()"
"\n"	"#  define mstackone()"
"\n"	"# else"
"\n"	"LOCALLIB void mstacktwo() { if (main_sp < 2) mstackempty(); }"
"\n"	"LOCALLIB void mstackone() { if (main_sp < 1) mstackempty(); }"
"\n"	"# endif"
"\n"
"\n"	"#define TOS (main_stack[main_sp-1])"
"\n"	"#define NOS (main_stack[main_sp-2])"
"\n"
"\n"	"LOCALLIB void ws_push(cell_t val) {"
"\n"	"    if (main_sp >= main_slen) mextend();"
"\n"	"    main_stack[main_sp++] = val;"
"\n"	"}"
"\n"
"\n"	"LOCALLIB cell_t ws_pop() {"
"\n"	"    mstackone();"
"\n"	"    return main_stack[--main_sp];"
"\n"	"}"
"\n"
"\n"	"LOCALLIB void ws_drop() { ws_pop(); }"
"\n"	"LOCALLIB void ws_dup() { mstackone(); ws_push(TOS); }"
"\n"	"LOCALLIB void ws_swap() { cell_t t; mstacktwo(); t = TOS; TOS = NOS; NOS = t; }"
"\n"	"LOCALLIB void ws_add() { mstacktwo(); NOS = NOS + TOS; main_sp--; }"
"\n"	"LOCALLIB void ws_sub() { mstacktwo(); NOS = NOS - TOS; main_sp--; }"
"\n"	"LOCALLIB void ws_mul() { mstacktwo(); NOS = NOS * TOS; main_sp--; }"
"\n"	"LOCALLIB void ws_div() { mstacktwo(); if (TOS) NOS = NOS / TOS; else NOS=0; main_sp--; }"
"\n"	"LOCALLIB void ws_mod() { mstacktwo(); if (TOS) NOS = NOS % TOS; else NOS=0; main_sp--; }"
"\n"
"\n"	"LOCALLIB void ws_store() {"
"\n"	"    mstacktwo();"
"\n"	"    extend_memory(NOS);"
"\n"	"    memory[NOS] = TOS;"
"\n"	"    main_sp -= 2;"
"\n"	"}"
"\n"
"\n"	"LOCALLIB void ws_fetch() {"
"\n"	"    mstackone();"
"\n"	"    if (TOS < 0 || TOS >= memlen) {"
"\n"	"# ifndef NO_MEMORYCHECK"
"\n"	"\tcheck_memory(TOS);"
"\n"	"# endif"
"\n"	"\tTOS = 0;"
"\n"	"\treturn;"
"\n"	"    }"
"\n"	"    TOS = memory[TOS];"
"\n"	"}"
"\n"
"\n"	"LOCALLIB void ws_outc() { mstackone(); putchar(TOS); main_sp--; }"
"\n"
"\n"	"LOCALLIB void ws_readc() {"
"\n"	"    mstackone();"
"\n"	"    extend_memory(TOS);"
"\n"	"    memory[TOS] = getchar();"
"\n"	"    main_sp--;"
"\n"	"}"
"\n"
"\n"	"static GCC_UNUSED void ws_outn() { mstackone(); printf(\"%\"PRIdcell, TOS); main_sp--; }"
"\n"
"\n"	"static GCC_UNUSED void ws_readn() {"
"\n"	"    char ibuf[1024];"
"\n"	"    mstackone();"
"\n"	"    extend_memory(TOS);"
"\n"	"    if (fgets(ibuf, sizeof(ibuf), stdin)) {"
"\n"	"\tcell_t val = 0;"
"\n"	"\tsscanf(ibuf, \"%\"SCNdcell, &val);"
"\n"	"\tmemory[TOS] = val;"
"\n"	"    } else"
"\n"	"\tmemory[TOS] = 0;"
"\n"	"    main_sp--;"
"\n"	"}"
"\n"
"\n"	"static GCC_UNUSED void ws_pick(cell_t val) {"
"\n"	"    if(val < 0 || val >= main_sp) mstackempty();"
"\n"	"    ws_push(main_stack[main_sp-val-1]);"
"\n"	"}"
"\n"
"\n"	"static GCC_UNUSED void ws_slide(cell_t val) {"
"\n"	"    cell_t t;"
"\n"	"    mstackone();"
"\n"	"    t = TOS;"
"\n"	"    while(val-->0 && main_sp) ws_drop();"
"\n"	"    mstackone();"
"\n"	"    TOS = t;"
"\n"	"}"
"\n"
"\n"	"int"
"\n"	"main(void)"
"\n"	"{"
"\n"	"    setbuf(stdout, 0);"
"\n"	"    rpush(-1);"
"\n"
"\n"	"    while(1) {"
"\n"	"\tswitch(rpop()) {"
"\n"	"\tcase -1:;"
"\n"
"\n"	"#define ws_trailer } ws_exit(); } }"

;
