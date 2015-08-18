%{
#include <stdio.h>
#define YY_INPUT(buf,result,max_size) result = ws_prefilter(buf,max_size)
size_t ws_prefilter(char * buf, size_t maxsize);

int cv_number(char *);
char * cv_label(char *);
char * cv_chr(char *);
%}
    /* Not using these, make GCC STFU. */
%option noinput
%option nounput

%option noyywrap

wsnum	[\t ]+\n
wslbl	[\t ]*\n

%%

"\n\n\n"		printf("ws_exit();\t/* %s */\n", cv_chr(yytext));

"\n \n"{wslbl}		printf("ws_jump(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
"\n  "{wslbl}		printf("ws_label(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
"\n \t"{wslbl}		printf("ws_call(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));

"\n\t\n"		printf("ws_ret();\t/* %s */\n", cv_chr(yytext));
"\n\t "{wslbl}		printf("ws_jz(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));
"\n\t\t"{wslbl}		printf("ws_jn(%s);\t/* %s */\n", cv_label(yytext+3), cv_chr(yytext));

" \n\n"			printf("ws_drop();\t/* %s */\n", cv_chr(yytext));
" \n "			printf("ws_dup();\t/* %s */\n", cv_chr(yytext));
" \n\t"			printf("ws_swap();\t/* %s */\n", cv_chr(yytext));

"  "{wsnum}		printf("ws_push(%d);\t/* %s */\n", cv_number(yytext+2), cv_chr(yytext));

" \t\n"{wsnum}		printf("ws_leave(%d);\t/* %s */\n", cv_number(yytext+3), cv_chr(yytext));
" \t "{wsnum}		printf("ws_pick(%d);\t/* %s */\n", cv_number(yytext+3), cv_chr(yytext));

"\t\n  "		printf("ws_outc();\t/* %s */\n", cv_chr(yytext));
"\t\n \t"		printf("ws_outn();\t/* %s */\n", cv_chr(yytext));
"\t\n\t "		printf("ws_readc();\t/* %s */\n", cv_chr(yytext));
"\t\n\t\t"		printf("ws_readn();\t/* %s */\n", cv_chr(yytext));

"\t  \n"		printf("ws_mul();\t/* %s */\n", cv_chr(yytext));
"\t   "			printf("ws_add();\t/* %s */\n", cv_chr(yytext));
"\t  \t"		printf("ws_sub();\t/* %s */\n", cv_chr(yytext));
"\t \t "		printf("ws_div();\t/* %s */\n", cv_chr(yytext));
"\t \t\t"		printf("ws_mod();\t/* %s */\n", cv_chr(yytext));

"\t\t "			printf("ws_store();\t/* %s */\n", cv_chr(yytext));
"\t\t\t"		printf("ws_fetch();\t/* %s */\n", cv_chr(yytext));

    /* These are unallocated sequences */
"\n\n "			|
"\n\n\t"		|
"  \n"			|
" \t\t"			|
"\t\n\n"		|
"\t\n \n"		|
"\t\n\t\n"		|
"\t \n"			|
"\t \t\n"		|
"\t\t\n"		printf("ws_%s();\n", cv_chr(yytext));

"\t"			printf("/* stray tab */\n");
" "			printf("/* stray space */\n");
"\n"			printf("/* stray linefeed */\n");

.               	/* Already filtered. */;
%%

/* TODO:
    Make pretty labels.
    Add fallbacks for labels that are not defined.
    Options to include debug information.
    Option to trap stack errors.
    Option to expand memory on read (or read zeros)

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
	    yyrestart(yyin);
	    BEGIN(INITIAL);
	}

	yylex();

	if(argc>1) {
	    fclose(yyin);
	    argv++, argc--;
	}
    } while(argc>1);

    printf("\nws_trailer\n");
    return 0;
}

size_t
ws_prefilter(char * buf, size_t max_size)
{
    size_t n;
    int c;
    for ( n = 0; n < max_size && (c = getc( yyin )) != EOF ;) {
	if (c == ' ' || c == '\t' || c == '\n')
	    buf[n++] = (char) c;
    }
    if ( c == EOF && ferror( yyin ) )
	YY_FATAL_ERROR( "input in flex scanner failed" );
    return n;
}

int cv_number(char * ws_num)
{
    int negative = (*ws_num++ != ' ');
    int value = 0;

    if (strlen(ws_num) > 32)
	YY_FATAL_ERROR( "Literal constant too large" );

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
    for (s = ws_label; *s; s++) switch(*s)
    {
	case ' ': *s = '0'; break;
	case '\t': *s = '1'; break;
	case '\n': *s = '\0'; break;
	default: *s = '*'; break;
    }
    return ws_label;
}

char * cv_chr(char * ws_code)
{
    char * s;
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
"\n"	"cell_t * main_stack = 0, tos = 0, nos = 0;"
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
"\n"	"    if(retr_sp == 0) return 0;"
"\n"	"    return retr_stack[--retr_sp];"
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
"\n"	"LOCALLIB void mpush(cell_t val) {"
"\n"	"    if (main_sp >= main_slen) mextend();"
"\n"	"    main_stack[main_sp++] = val;"
"\n"	"}"
"\n"
"\n"	"LOCALLIB cell_t mpop() {"
"\n"	"    if(main_sp == 0) return 0;"
"\n"	"    return main_stack[--main_sp];"
"\n"	"}"
"\n"
"\n"	"LOCALLIB void ws_push(cell_t val) { mpush(nos); nos = tos; tos = val; }"
"\n"	"LOCALLIB void ws_drop() { tos = nos; nos = mpop(); }"
"\n"	"LOCALLIB cell_t ws_pop() { cell_t val = tos; ws_drop(); return val; }"
"\n"	"LOCALLIB void ws_dup() { mpush(nos); nos = tos; }"
"\n"	"LOCALLIB void ws_swap() { cell_t t = tos; tos = nos; nos = t; }"
"\n"	"LOCALLIB void ws_add() { tos = nos + tos; nos = mpop(); }"
"\n"	"LOCALLIB void ws_sub() { tos = nos - tos; nos = mpop(); }"
"\n"	"LOCALLIB void ws_mul() { tos = nos * tos; nos = mpop(); }"
"\n"	"LOCALLIB void ws_div() { if (tos) tos = nos / tos; nos = mpop(); }"
"\n"	"LOCALLIB void ws_mod() { if (tos) tos = nos % tos; nos = mpop(); }"
"\n"
"\n"	"LOCALLIB void ws_store() {"
"\n"	"    extend_memory(nos);"
"\n"	"    memory[nos] = tos;"
"\n"	"    ws_drop(); ws_drop();"
"\n"	"}"
"\n"
"\n"	"LOCALLIB void ws_fetch() { check_memory(tos); tos = memory[tos]; }"
"\n"
"\n"	"LOCALLIB void ws_outc() { putchar(tos); ws_drop(); }"
"\n"
"\n"	"LOCALLIB void ws_readc() {"
"\n"	"    extend_memory(tos);"
"\n"	"    memory[tos] = getchar();"
"\n"	"    ws_drop();"
"\n"	"}"
"\n"
"\n"	"static GCC_UNUSED void ws_outn() { printf(\"%\"PRIdcell, tos); ws_drop(); }"
"\n"
"\n"	"static GCC_UNUSED void ws_readn() {"
"\n"	"    char ibuf[1024];"
"\n"	"    extend_memory(tos);"
"\n"	"    if (fgets(ibuf, sizeof(ibuf), stdin)) {"
"\n"	"\tcell_t val = 0;"
"\n"	"\tsscanf(ibuf, \"%\"SCNdcell, &val);"
"\n"	"\tmemory[tos] = val;"
"\n"	"    } else"
"\n"	"\tmemory[tos] = 0;"
"\n"	"    ws_drop();"
"\n"	"}"
"\n"
"\n"	"static GCC_UNUSED void ws_pick(cell_t val) {"
"\n"	"    ws_dup();"
"\n"	"    if(val <= 0) return;"
"\n"	"    if(val >= main_sp) return;"
"\n"	"    tos = main_stack[main_sp-val];"
"\n"	"}"
"\n"
"\n"	"static GCC_UNUSED void ws_leave(cell_t val) {"
"\n"	"    cell_t t = tos;"
"\n"	"    while(val-->0 && main_sp) ws_drop();"
"\n"	"    tos = t;"
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
