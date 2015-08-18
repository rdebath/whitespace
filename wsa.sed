
1i\
#include "ws_gencode.h"\
main() {
$a\
}

s/	/    /g
s/^\([^";]*\)--/\1;/
s/{\-/\/\*/g
s/\-}/\*\//g

h
s/^[^;]*//
s/;/\/\//
x

s/;.*//

s/^ *doub *$/ws_dup();/
s/^ *outN *$/ws_outn();/i
s/^ *retrive *$/ws_fetch();/
s/^ *store *$/ws_store();/
s/^ *swap *$/ws_swap();/
s/^ *ret *$/ws_return();/
s/^ *exit *$/ws_exit();/
s/^ *outC *$/ws_outc();/i
s/^ *pop *$/ws_drop();/

s/^ *InC *$/ws_readc();/i
s/^ *InN *$/ws_readn();/i

s/^ *call *\([A-Za-z][A-Za-z0-9_]*\)[    ]*$/ws_call(atom("\1"));/
s/^ *jump *\([A-Za-z][A-Za-z0-9_]*\)[    ]*$/ws_jump(atom("\1"));/
s/^ *jumpz *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/ws_jz(atom("\1"));/
s/^ *jumpn *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/ws_jn(atom("\1"));/
s/^ *label *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/ws_label(atom("\1"));/

s/^ *jumppz *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/ws_jpz(atom("\1"));/
s/^ *jumpnz *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/ws_jnz(atom("\1"));/
s/^ *jumpp *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/ws_jp(atom("\1"));/

s/^ *pushs *\("[^"]*"\) *$/ws_pushs(\1);/
s/^ *push *\([0-9][0-9]*\)/ws_push(\1);/
s/^ *push *\-\([0-9][0-9]*\)/ws_push(-\1);/

s/^ *add *\([0-9][0-9]*\) *$/ws_push(\1); ws_add();/
s/^ *sub *\([0-9][0-9]*\) *$/ws_push(\1); ws_sub();/

s/^ *store *\([0-9][0-9]*\) *$/ws_push(\1); ws_swap(); ws_store();/

s/^ *retrive *\([0-9][0-9]*\) *$/ws_push(\1); ws_fetch();/
s/^ *test *\([0-9][0-9]*\) *$/ws_dup(); ws_push(\1); ws_sub();/

s/^ *add *$/ws_add();/
s/^ *mul *$/ws_mul();/
s/^ *sub *$/ws_sub();/
s/^ *div *$/ws_div();/
s/^ *mod *$/ws_mod();/

# s/^ *ifoption *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/if(\1) {/
# s/^ *endoption *$/}/

s/^ *ifoption *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/#ifdef \1/
s/^ *endoption *$/#endif/
s/^ *include *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/#include "\1.h"/

s/^ *debug_printheap *$/\/\/ debug_printheap/
s/^ *debug_printstack *$/\/\/ debug_printstack/

G
s/\n//
