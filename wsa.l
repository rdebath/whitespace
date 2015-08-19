%{
#include <stdio.h>

%}
    /* Not using these, make GCC STFU. */
%option noinput
%option nounput

%option noyywrap
%option yylineno

    /* Lots of states becaue I'm parsing in here rather than yacc/bison */
%s BODY
%s EOL
%s NUMARG
%s LBLARG
%x ERROR

alpha   [A-Za-z]
dig     [0-9]
name    ({alpha}|[_$])({alpha}|{dig}|[_$])*

digs    {dig}+
w	[ \t]*

%%

<INITIAL>{
{digs}:		|
\.{name}{w}:	|
{name}{w}:	{ printf("ws_label(%.*s);\n", yyleng-1, yytext); BEGIN(BODY); }
}

<INITIAL,BODY>{
(quit|exit|end)		{ printf("ws_exit();\n"); BEGIN(EOL); }
(drop|discard)		{ printf("ws_drop();\n"); BEGIN(EOL); }
(ret|return)		{ printf("ws_return();\n"); BEGIN(EOL); }
(fetch|retrieve|retrive|retreive)	{ printf("ws_fetch();\n"); BEGIN(EOL); }
dup			{ printf("ws_dup();\n"); BEGIN(EOL); }
swap			{ printf("ws_swap();\n"); BEGIN(EOL); }
(outc|outchar|printc)	{ printf("ws_outc();\n"); BEGIN(EOL); }
(outn|outnum|printi)	{ printf("ws_outn();\n"); BEGIN(EOL); }
(readc|readchar)	{ printf("ws_readc();\n"); BEGIN(EOL); }
(readn|readnum|readi)	{ printf("ws_readn();\n"); BEGIN(EOL); }
store			{ printf("ws_store();\n"); BEGIN(EOL); }
mul			{ printf("ws_mul();\n"); BEGIN(EOL); }
add			{ printf("ws_add();\n"); BEGIN(EOL); }
sub			{ printf("ws_sub();\n"); BEGIN(EOL); }
div			{ printf("ws_div();\n"); BEGIN(EOL); }
mod			{ printf("ws_mod();\n"); BEGIN(EOL); }

slide		{ printf("ws_slide("); BEGIN(NUMARG); }
(copy|pick)	{ printf("ws_pick("); BEGIN(NUMARG); }
push		{ printf("ws_push("); BEGIN(NUMARG); }

(jmp|jump)	{ printf("ws_jump("); BEGIN(LBLARG); }
label		{ printf("ws_label("); BEGIN(LBLARG); }
call		{ printf("ws_call("); BEGIN(LBLARG); }
jz		{ printf("ws_jz("); BEGIN(LBLARG); }
jn		{ printf("ws_jn("); BEGIN(LBLARG); }

}

<NUMARG>{
-?[0-9]+	{ printf("%s);\n", yytext); BEGIN(EOL); }

'[^\\]'		|
'\\[ntab']'	{ printf("%s);\n", yytext); BEGIN(EOL); }

\n		{ printf("); Error missing value\n"); BEGIN(INITIAL); }
}

<LBLARG>{
{digs}		|
{name}		|
\.{name}	{ printf("%s);\n", yytext); BEGIN(EOL); }

\n		{ printf("); Error missing label\n"); BEGIN(INITIAL); }
}

<ERROR>.*\n	{ BEGIN(INITIAL); }

;.*		; /* Line comment */
#.*		; /* Line comment */
[ \t]+		; /* Whitespace */

"\n"		{ BEGIN(INITIAL); }

.               |
{name}		{ printf("Error badword \"%s\" on line %d\n", yytext, yylineno); BEGIN(ERROR); }
%%

/* TODO:
    Add meta commands:
	print string
	set memory eg: word ascii asciiz
	Command for putting special \n\s\t sequences in output.
	Codes mering push or push/swap into next instruction. (add/fetch etc).

    Code generation
    Label checking, label sorting (by usage)
    Mark/scan for dead code.
    "Start" command for entry point.

    Quick call ($name)
    Data labels, use for store and fetch commands.
	lblname = word 0

*/

int main(int argc, char ** argv)
{
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
