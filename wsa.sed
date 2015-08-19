
1i\
#define ATOMISE_LABELS\
#include "ws_gencode.h"\
int main() {
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

s/^\([ 	]*\)doub *$/\1ws_dup();/
s/^\([ 	]*\)outN *$/\1ws_outn();/i
s/^\([ 	]*\)retrive *$/\1ws_fetch();/
s/^\([ 	]*\)store *$/\1ws_store();/
s/^\([ 	]*\)swap *$/\1ws_swap();/
s/^\([ 	]*\)ret *$/\1ws_return();/
s/^\([ 	]*\)exit *$/\1ws_exit();/
s/^\([ 	]*\)outC *$/\1ws_outc();/i
s/^\([ 	]*\)pop *$/\1ws_drop();/

s/^\([ 	]*\)InC *$/\1ws_readc();/i
s/^\([ 	]*\)InN *$/\1ws_readn();/i

s/^\([ 	]*\)call *\([A-Za-z][A-Za-z0-9_]*\)[    ]*$/\1ws_call(\2);/
s/^\([ 	]*\)jump *\([A-Za-z][A-Za-z0-9_]*\)[    ]*$/\1ws_jump(\2);/
s/^\([ 	]*\)jumpz *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jz(\2);/
s/^\([ 	]*\)jumpn *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jn(\2);/
s/^\([ 	]*\)label *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_label(\2);/

s/^\([ 	]*\)jumppz *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jzp(\2);/
s/^\([ 	]*\)jumpnz *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jzn(\2);/
s/^\([ 	]*\)jumpp *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jp(\2);/
s/^\([ 	]*\)jumppn *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jnz(\2);/
s/^\([ 	]*\)jumpnp *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/\1ws_jnz(\2);/

s/^\([ 	]*\)pushs *\("[^"]*"\) *$/\1ws_pushs(\2);/
s/^\([ 	]*\)push *\([0-9][0-9]*\)/\1ws_push(\2);/
s/^\([ 	]*\)push *\-\([0-9][0-9]*\)/\1ws_push(-\2);/

s/^\([ 	]*\)add *\([0-9][0-9]*\) *$/\1ws_push(\2); ws_add();/
s/^\([ 	]*\)sub *\([0-9][0-9]*\) *$/\1ws_push(\2); ws_sub();/

s/^\([ 	]*\)store *\([0-9][0-9]*\) *$/\1ws_push(\2); ws_swap(); ws_store();/

s/^\([ 	]*\)retrive *\([0-9][0-9]*\) *$/\1ws_push(\2); ws_fetch();/
s/^\([ 	]*\)test *\([0-9][0-9]*\) *$/\1ws_dup(); ws_push(\2); ws_sub();/

s/^\([ 	]*\)add *$/\1ws_add();/
s/^\([ 	]*\)mul *$/\1ws_mul();/
s/^\([ 	]*\)sub *$/\1ws_sub();/
s/^\([ 	]*\)div *$/\1ws_div();/
s/^\([ 	]*\)mod *$/\1ws_mod();/

# s/^\([ 	]*\)ifoption *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/if(\2) {/
# s/^\([ 	]*\)endoption *$/}/

s/^\([ 	]*\)ifoption *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/#ifdef \2/
s/^\([ 	]*\)endoption *$/#endif/
s/^\([ 	]*\)include *\([A-Za-z][A-Za-z0-9_]*\)[   ]*$/#include "\2.h"/

s/^\([ 	]*\)debug_printheap *$/\/\/ \1debug_printheap()/
s/^\([ 	]*\)debug_printstack *$/\/\/ \1debug_printstack()/

G
s/\n//
