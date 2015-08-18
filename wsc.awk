BEGIN{ b="";
    print_template();
}
END{ 
    for(lbl in undefinedlabs) {
	print "ws_exit();";
	print "ws_label(" lbl ");";
	print "fprintf(stderr, \"Undefined label \\\"" lbl "\\\" called\\n\");"
    }
    print "\nws_trailer";
    if (b != "")
	print "/* Leftovers:",b,"*/"
}
{
    s=$0;
    gsub("[^\n\t ]", "", s);
    gsub("\t", "t", s);
    gsub(" ", "s", s);
    b = b s "l";

    while (length(b) >= 3) {

	w = substr(b, 1, 3);
	b = substr(b, 4);

	if (w == "sls") {
	    print "ws_dup();\t/*",w,"*/";
	} else if (w == "slt") {
	    print "ws_swap();\t/*",w,"*/";
	} else if (w == "sll") {
	    print "ws_drop();\t/*",w,"*/";
	} else if (w == "tts") {
	    print "ws_store();\t/*",w,"*/";
	} else if (w == "ttt") {
	    print "ws_fetch();\t/*",w,"*/";
	} else if (w == "ltl") {
	    print "ws_ret();\t/*",w,"*/";
	} else if (w == "lll") {
	    print "ws_exit();\t/*",w,"*/";
	} else if (w == "ssl") {
	    print "ws_push(0);\t/*",w,"*/";
	} else {
	    if (b == "") { b = w b; next; }

	    b = w b;
	    w = substr(b, 1, 4);
	    b = substr(b, 5);
	    if (w == "tsss") {
		print "ws_add();\t/*",w,"*/";
	    } else if (w == "tsst") {
		print "ws_sub();\t/*",w,"*/";
	    } else if (w == "tssl") {
		print "ws_mul();\t/*",w,"*/";
	    } else if (w == "tsts") {
		print "ws_div();\t/*",w,"*/";
	    } else if (w == "tstt") {
		print "ws_mod();\t/*",w,"*/";
	    } else if (w == "tlss") {
		print "ws_outc();\t/*",w,"*/";
	    } else if (w == "tlst") {
		print "ws_outn();\t/*",w,"*/";
	    } else if (w == "tlts") {
		print "ws_readc();\t/*",w,"*/";
	    } else if (w == "tltt") {
		print "ws_readn();\t/*",w,"*/";
	    } else {
		b = w b;
		w = substr(b, 1, 3);
		b = substr(b, 4);
		i = index(b, "l");
		l = substr(b, 1, i-1);
		b = substr(b, i+1);
		if (w == "sss") {
		    print "ws_push(" bintoi(l) ");\t/*",w l "l */";
		} else if (w == "sst") {
		    print "ws_push(-" bintoi(l) ");\t/*",w l "l */";
		} else if (w == "lss") {
		    lbl = bintol(l);
		    definedlabs[lbl] = 1;
		    if (lbl in undefinedlabs) delete undefinedlabs[lbl]
		    print "ws_label(" lbl ");\t/*",w l "l */";
		} else if (w == "lst") {
		    print "ws_call(" bintol(l) ");\t/*",w l "l */";
		} else if (w == "lsl") {
		    print "ws_jump(" bintol(l) ");\t/*",w l "l */";
		} else if (w == "lts") {
		    print "ws_jz(" bintol(l) ");\t/*",w l "l */";
		} else if (w == "ltt") {
		    print "ws_jn(" bintol(l) ");\t/*",w l "l */";
		} else if (w == "sts" && substr(l, 1, 1) == "s") {
		    print "ws_pick(" bintoi(l) ");\t/*",w l "l */";
		} else if (w == "stl" && substr(l, 1, 1) == "s") {
		    print "ws_leave(" bintoi(l) ");\t/*",w l "l */";
		} else {
		    print "if (ws_" w ") ws_" w "(\"" l "\");\t/*",w l "l */";
		}
	    }
	}
    }
}

function bintoi(l, i) {
    i = 0;
    while(l != "") {
	if(substr(l, 1, 1) == "s") {
	    i = i * 2;
	} else {
	    i = i * 2 + 1;
	}
	l = substr(l, 2);
    }
    return i;
}

function bintol(l, nl, x, a, c) {
    # TODO: this should be reversible; might already be.
    if ((length(l) % 8) == 0 && length(l)>0) {
	nl = "";
	a = l;
	c = 1;
	while(l != "" && c) {
	    x = substr(l, 1, 8);
	    x = bintoi(x);
	    if ( x>=48 && x<=57 && nl != "") {
		nl = nl sprintf("%c", x);
	    } else if ( (x>=65 && x<=90) || (x>=97 && x<=122) || x==95) {
		nl = nl sprintf("%c", x);
	    } else
		c = 0;
	    l = substr(l, 9);
	}
	if (c) {
	    if (!(nl in definedlabs)) undefinedlabs[nl] = 1;
	    return nl;
	}
	l = a;
    }

    nl = "";
    if (l == "") nl = "0x0";
    while(l != "") {
	a = length(l);
	x = substr(l, a-23, 24);
	c = bintoi(x);

	if (nl != "") nl = "_" nl;
	if (substr(x, 1, 1) == "t") {
	    nl = sprintf("%d", c) nl;
	} else if (substr(x, 1, 2) == "st") {
	    nl = sprintf("0d%d", c) nl;
	} else {
	    nl = sprintf("%dx%d", length(x), c) nl;
	}

	l = substr(l, 1, a-24);
    }
    if (!(nl in definedlabs)) undefinedlabs[nl] = 1;
    return nl;
}

function print_template() {

print	"#include <stdio.h>"
print	"#include <stdlib.h>"
print	"#include <unistd.h>"
print	""
print	"# ifndef NO_INTTYPES"
print	"#  include <inttypes.h>"
print	"#  define cell_t\t\tintmax_t"
print	"#  define INTcell_C(mpm)\tINTMAX_C(mpm)"
print	"#  define PRIdcell\t\tPRIdMAX"
print	"#  define SCNdcell\t\tSCNdMAX"
print	"# else"
print	"#  define cell_t\t\tlong"
print	"#  define INTcell_C(mpm)\tmpm ## L"
print	"#  define PRIdcell\t\t\"ld\""
print	"#  define SCNdcell\t\t\"ld\""
print	"# endif"
print	""
print	"# ifdef __GNUC__"
print	"#  define GCC_UNUSED __attribute__ ((unused))"
print	"# else"
print	"#  define GCC_UNUSED"
print	"# endif"
print	""
print	"#define LOCALLIB static inline GCC_UNUSED"
print	""
print	"#define ws_label(x)\tL_ ## x:"
print	"#define ws_call(x)\trpush(__LINE__); goto L_ ## x; case __LINE__:"
print	"#define ws_jump(x)\tgoto L_ ## x"
print	"#define ws_jz(x)\tif(ws_pop() == 0) goto L_ ## x"
print	"#define ws_jn(x)\tif(ws_pop() < 0) goto L_ ## x"
print	"#define ws_ret()\tcontinue"
print	"#define ws_exit()\texit(0)"
print	""
print	"cell_t * main_stack = 0, tos = 0, nos = 0;"
print	"int main_slen = 0, main_sp = 0;"
print	""
print	"int * retr_stack = 0;"
print	"int retr_slen = 0, retr_sp = 0;"
print	""
print	"cell_t *memory = 0;"
print	"int memlen = 0;"
print	""
print	"static void check_memory(cell_t mptr) {"
print	"    if (mptr < 0) {"
print	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" is illegal.\\n\", mptr);"
print	"\texit(99);"
print	"    }"
print	"    if (mptr < 0 || mptr >= memlen) {"
print	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" has not been allocated.\\n\", mptr);"
print	"\texit(2);"
print	"    }"
print	"}"
print	""
print	"static void extend_memory(cell_t mptr) {"
print	"    int oldmsize, newmsize, i;"
print	"    if (mptr < 0) {"
print	"\tfprintf(stderr, \"Memory pointer %\"PRIdcell\" is illegal.\\n\", mptr);"
print	"\texit(99);"
print	"    }"
print	"    if (mptr < memlen) return;"
print	""
print	"    oldmsize = ((memlen+4095)/4096)*4096;"
print	"    memlen = mptr + 1;"
print	"    newmsize = ((memlen+4095)/4096)*4096;"
print	"    if (oldmsize == newmsize) return;"
print	"    memory = realloc(memory, newmsize*sizeof*memory);"
print	"    if (memory == 0) {"
print	"\tfprintf(stderr, \"Out of memory: %\"PRIdcell\"\\n\", mptr);"
print	"\texit(4);"
print	"    }"
print	"    for(i=oldmsize; i<newmsize; i++)"
print	"\tmemory[i] = 0;"
print	"}"
print	""
print	"static void rextend(void) {"
print	"    retr_stack = realloc(retr_stack, (retr_slen += 1024)*sizeof*retr_stack);"
print	"    if (!retr_stack) {"
print	"\tperror(\"wsi: return stack\");"
print	"\texit(1);"
print	"    }"
print	"}"
print	""
print	"LOCALLIB void rpush(int val) {"
print	"    if (retr_sp >= retr_slen) rextend();"
print	"    retr_stack[retr_sp++] = val;"
print	"}"
print	""
print	"LOCALLIB int rpop() {"
print	"    if(retr_sp == 0) return 0;"
print	"    return retr_stack[--retr_sp];"
print	"}"
print	""
print	"static void mextend(void) {"
print	"    main_stack = realloc(main_stack, (main_slen += 1024)*sizeof*main_stack);"
print	"    if (!main_stack) {"
print	"\tperror(\"wsi: main stack\");"
print	"\texit(1);"
print	"    }"
print	"}"
print	""
print	"LOCALLIB void mpush(cell_t val) {"
print	"    if (main_sp >= main_slen) mextend();"
print	"    main_stack[main_sp++] = val;"
print	"}"
print	""
print	"LOCALLIB cell_t mpop() {"
print	"    if(main_sp == 0) return 0;"
print	"    return main_stack[--main_sp];"
print	"}"
print	""
print	"LOCALLIB void ws_push(cell_t val) { mpush(nos); nos = tos; tos = val; }"
print	"LOCALLIB void ws_drop() { tos = nos; nos = mpop(); }"
print	"LOCALLIB cell_t ws_pop() { cell_t val = tos; ws_drop(); return val; }"
print	"LOCALLIB void ws_dup() { mpush(nos); nos = tos; }"
print	"LOCALLIB void ws_swap() { cell_t t = tos; tos = nos; nos = t; }"
print	"LOCALLIB void ws_add() { tos = nos + tos; nos = mpop(); }"
print	"LOCALLIB void ws_sub() { tos = nos - tos; nos = mpop(); }"
print	"LOCALLIB void ws_mul() { tos = nos * tos; nos = mpop(); }"
print	"LOCALLIB void ws_div() { if (tos) tos = nos / tos; nos = mpop(); }"
print	"LOCALLIB void ws_mod() { if (tos) tos = nos % tos; nos = mpop(); }"
print	""
print	"LOCALLIB void ws_store() {"
print	"    extend_memory(nos);"
print	"    memory[nos] = tos;"
print	"    ws_drop(); ws_drop();"
print	"}"
print	""
print	"LOCALLIB void ws_fetch() { check_memory(tos); tos = memory[tos]; }"
print	""
print	"LOCALLIB void ws_outc() { putchar(tos); ws_drop(); }"
print	""
print	"LOCALLIB void ws_readc() {"
print	"    extend_memory(tos);"
print	"    memory[tos] = getchar();"
print	"    ws_drop();"
print	"}"
print	""
print	"static GCC_UNUSED void ws_outn() { printf(\"%\"PRIdcell, tos); ws_drop(); }"
print	""
print	"static GCC_UNUSED void ws_readn() {"
print	"    char ibuf[1024];"
print	"    extend_memory(tos);"
print	"    if (fgets(ibuf, sizeof(ibuf), stdin)) {"
print	"\tcell_t val = 0;"
print	"\tsscanf(ibuf, \"%\"SCNdcell, &val);"
print	"\tmemory[tos] = val;"
print	"    } else"
print	"\tmemory[tos] = 0;"
print	"    ws_drop();"
print	"}"
print	""
print	"static GCC_UNUSED void ws_pick(cell_t val) {"
print	"    ws_dup();"
print	"    if(val <= 0) return;"
print	"    if(val >= main_sp) return;"
print	"    tos = main_stack[main_sp-val];"
print	"}"
print	""
print	"static GCC_UNUSED void ws_leave(cell_t val) {"
print	"    cell_t t = tos;"
print	"    while(val-->0 && main_sp) ws_drop();"
print	"    tos = t;"
print	"}"
print	""
print	"int"
print	"main(void)"
print	"{"
print	"    setbuf(stdout, 0);"
print	"    rpush(-1);"
print	""
print	"    while(1) {"
print	"\tswitch(rpop()) {"
print	"\tcase -1:"
print	""
print	"#define ws_trailer } ws_exit(); } }"
print	""

}
