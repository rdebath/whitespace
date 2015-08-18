
#include "ws_gencode.h"

main()
{
// get prim numbers

ws_pushs("prim numbers [2-100]");
ws_call(atom("printsln"));



ws_push(0);
//for loop
ws_label(atom("f_start_1"));
ws_dup(); ws_push(100); ws_sub();
ws_jz(atom("f_finished_1"));

ws_dup();
ws_push(100); ws_add();
        ws_push(1);
ws_store();

ws_push(1); ws_add();
ws_jump(atom("f_start_1"));

ws_label(atom("f_finished_1"));
ws_drop();

//debug_printstack
//debug_printheap

ws_push(2);
//for loop
ws_label(atom("f_start_2"));
ws_dup(); ws_push(11); ws_sub();
ws_jz(atom("f_finished_2"));



        ws_push(2);
        //for loop
        ws_label(atom("f_start_3"));
ws_dup(); ws_push(100); ws_sub();
        ws_jz(atom("f_finished_3"));

ws_dup();
                        ws_push(1);
ws_swap();
ws_store();

ws_swap();

ws_dup();
                        ws_push(2);
ws_swap();
ws_store();

ws_swap();

ws_push(1); ws_fetch();
ws_push(2); ws_fetch();
ws_mul();
ws_push(100); ws_add();

                ws_push(0);
ws_store();

ws_push(1); ws_add();

        ws_jump(atom("f_start_3"));
        ws_label(atom("f_finished_3"));
        ws_drop();


ws_push(1); ws_add();

ws_jump(atom("f_start_2"));
ws_label(atom("f_finished_2"));
ws_drop();



ws_push(2);
//for loop
ws_label(atom("f_start_4"));
ws_dup(); ws_push(100); ws_sub();
ws_jz(atom("f_finished_4"));

ws_dup();
ws_push(100); ws_add();
ws_fetch();
ws_jz(atom("nodraw"));

ws_dup();
ws_outn();

ws_pushs(" ");
ws_call(atom("prints"));

ws_label(atom("nodraw"));

ws_push(1); ws_add();
ws_jump(atom("f_start_4"));

ws_label(atom("f_finished_4"));
ws_drop();

ws_push(0);
ws_call(atom("printsln"));

ws_exit();

ws_label(atom("prints"));

ws_dup();
ws_jz(atom("prints_end"));
ws_outc();
ws_jump(atom("prints"));

ws_label(atom("prints_end"));
ws_drop();
ws_return();

ws_label(atom("printsln"));
ws_call(atom("prints"));
ws_push(10);
ws_outc();
ws_return();


}
