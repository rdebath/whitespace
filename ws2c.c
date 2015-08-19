
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

cell_t cv_number(char *);
char * cv_label(char *);
char * cv_chr(char *);

void append_label(void);
void append_char(void);
void process_command(void);

FILE * yyin;

char * yytext;
int yytext_size = 0, yytext_len;

char * yylabel;
int yylabel_size = 0, yylabel_len;


/* TODO:
    Add fallbacks for labels that are not defined.
 */
char * header;

int main(int argc, char ** argv)
{
    printf("%s\n\n", header);

    do
    {
	if(argc>1) {
	    if (!(yyin = fopen(argv[1], "r"))) {
		perror(argv[1]);
		exit(1);
	    }
	}

	while (!feof(yyin) && !ferror(yyin))
	    process_command();

	if (ferror(yyin))
	    fprintf(stderr, "Error reading from file (ferror(3) so no reason given).\n");

	if(argc>1) {
	    fclose(yyin);
	    argv++, argc--;
	}
    } while(argc>1);

    printf("\nws_trailer\n");
    return 0;
}

void
process_command()
{
    yytext_len = 0;
    append_char();
    if (feof(yyin) || ferror(yyin)) return;

    append_char();
    append_char();

    if (yylabel_len != 0)
	printf("/* %s*/\n", yylabel);
    yylabel_len = 0;

    if (yytext[0] == ' ') {
	if (yytext[1] == ' ') {
	    if (yytext[2] != '\n') {
		append_label();
		printf("ws_push(%"PRIdcell");\t/* %s */\n", cv_number(yytext+2), cv_chr(yytext));
	    } else
		printf("ws_%s();\n", cv_chr(yytext));
	}
	if (yytext[1] == '\n') {
	    if (yytext[2] == ' ') printf("ws_dup();\t/* %s */\n", cv_chr(yytext));
	    if (yytext[2] == '\n') printf("ws_drop();\t/* %s */\n", cv_chr(yytext));
	    if (yytext[2] == '\t') printf("ws_swap();\t/* %s */\n", cv_chr(yytext));
	}
	if (yytext[1] == '\t') {
	    if (yytext[2] != '\t') {
		append_label();
		if (yytext[2] == ' ') printf("ws_pick(%"PRIdcell");\t/* %s */\n", cv_number(yytext+3), cv_chr(yytext));
		if (yytext[2] == '\n') printf("ws_slide(%"PRIdcell");\t/* %s */\n", cv_number(yytext+3), cv_chr(yytext));
	    } else
		printf("ws_%s();\n", cv_chr(yytext));
	}
    }

    if (yytext[0] == '\n') {
	if (yytext[1] == ' ') {
	    append_label();
	    if (yytext[2] == ' ') printf("ws_label(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
	    if (yytext[2] == '\n') printf("ws_jump(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
	    if (yytext[2] == '\t') printf("ws_call(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
	}
	if (yytext[1] == '\n') {
	    if (yytext[2] == '\n')
		printf("ws_exit();\t/* %s */\n", cv_chr(yytext));
	    else
		printf("ws_%s();\n", cv_chr(yytext));
	}
	if (yytext[1] == '\t') {
	    if (yytext[2] != '\n') {
		append_label();
		if (yytext[2] == ' ') printf("ws_jz(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
		if (yytext[2] == '\t') printf("ws_jn(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
	    } else
		printf("ws_ret();\t/* %s */\n", cv_chr(yytext));
	}
    }

    if (yytext[0] == '\t') {
	if (yytext[1] == ' ') {
	    if (yytext[2] == '\n')
		printf("ws_%s();\n", cv_chr(yytext));
	    else {
		append_char();
		if (yytext[2] == ' ') {
		    if (yytext[3] == ' ') printf("ws_add();\t/* %s */\n", cv_chr(yytext));
		    if (yytext[3] == '\n') printf("ws_mul();\t/* %s */\n", cv_chr(yytext));
		    if (yytext[3] == '\t') printf("ws_sub();\t/* %s */\n", cv_chr(yytext));
		}
		if (yytext[2] == '\t') {
		    if (yytext[3] == ' ') printf("ws_div();\t/* %s */\n", cv_chr(yytext));
		    if (yytext[3] == '\n') printf("ws_%s();\n", cv_chr(yytext));
		    if (yytext[3] == '\t') printf("ws_mod();\t/* %s */\n", cv_chr(yytext));
		}
	    }
	}
	if (yytext[1] == '\n') {
	    if (yytext[2] == '\n')
		printf("ws_%s();\n", cv_chr(yytext));
	    else {
		append_char();
		if (yytext[2] == ' ') {
		    if (yytext[3] == ' ') printf("ws_outc();\t/* %s */\n", cv_chr(yytext));
		    if (yytext[3] == '\n') printf("ws_%s();\n", cv_chr(yytext));
		    if (yytext[3] == '\t') printf("ws_outn();\t/* %s */\n", cv_chr(yytext));
		}
		if (yytext[2] == '\t') {
		    if (yytext[3] == ' ') printf("ws_readc();\t/* %s */\n", cv_chr(yytext));
		    if (yytext[3] == '\n') printf("ws_%s();\n", cv_chr(yytext));
		    if (yytext[3] == '\t') printf("ws_readn();\t/* %s */\n", cv_chr(yytext));
		}
	    }
	}
	if (yytext[1] == '\t') {
	    if (yytext[2] == ' ') printf("ws_store();\t/* %s */\n", cv_chr(yytext));
	    if (yytext[2] == '\n') printf("ws_%s();\n", cv_chr(yytext));
	    if (yytext[2] == '\t') printf("ws_fetch();\t/* %s */\n", cv_chr(yytext));
	}
    }

}

void
append_char() {
    int ch;
    for(;;)
    {
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
    if (ch == EOF) ch = '\n';
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
    do { append_char(); } while(yytext[yytext_len-1] != '\n');
}

cell_t
cv_number(char * ws_num)
{
    int negative = (*ws_num++ != ' ');
    cell_t value = 0;

    if (strlen(ws_num) > sizeof(cell_t) * 8) {
	fprintf(stderr, "WARNING: Literal constant too large: '..%s'\n", cv_chr(ws_num));
    }

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
    int l = -1;
    static char * sbuf = 0;
    static int maxlen = 0;

    for (s = ws_label; *s; s++) switch(*s)
    {
	case ' ': *s = '0'; break;
	case '\t': *s = '1'; break;
	case '\n': *s = '\0'; l = s-ws_label; break;
	default: *s = '*'; break;
    }
    if (l<0) return ws_label;	/* !? */
    if (l == 0) return "0x0";	/* An empty string is dubious. */

    if (l > maxlen) {
	if(sbuf) free(sbuf);
	sbuf = malloc(l+8);
	maxlen = l;
    }

    if (ws_label[0] == '0' && l%8 == 0 && l > 8) {
	int i = 0, j = 0, n = 0;
	for(s=ws_label; *s; s++) {
	    i *= 2;
	    if (*s == '1')
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
	if (n) {
	    sbuf[n] = 0;
	    return sbuf;
	}
    }

    if (ws_label[0] == '1' && l < 31) {
	int i = 0;
	for(s=ws_label; *s; s++) {
	    i *=2;
	    if (*s == '1')
		i ++;
	}
	sprintf(sbuf, "%d", i);
	return sbuf;
    }

    return ws_label;
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
    }
    sbuf[i] = 0;
    return sbuf;
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
"\n"	"#define ws_ret()\tcontinue"
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
"\n"	"static void check_memory(cell_t mptr) {"
"\n"	"    if (mptr < 0) {"
"\n"	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" is illegal.\\n\", mptr);"
"\n"	"\texit(99);"
"\n"	"    }"
"\n"	"    if (mptr < 0 || mptr >= memlen) {"
"\n"	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" has not been allocated.\\n\", mptr);"
"\n"	"\texit(2);"
"\n"	"    }"
"\n"	"}"
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
"\n"	"# ifdef NO_MEMORYCHECK"
"\n"	"    if (TOS < 0 || TOS >= memlen) { TOS = 0; return; }"
"\n"	"# else"
"\n"	"    check_memory(TOS);"
"\n"	"# endif"
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
"\n"	"\tcase -1:"
"\n"
"\n"	"#define ws_trailer } ws_exit(); } }"

;
