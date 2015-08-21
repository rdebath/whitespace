
#define ATOMISE_LABELS
#define BIJECTIVE 1

#include "ws_gencode.h"

ws_header();

// get prim numbers

ws_pushs("prim numbers [2-100]");
ws_call(printsln);


{
    // for(i=0; i<100; i++) memory[100+i] = 1;

    ws_push(0);
    //for loop
    ws_label(f_start_1);
    ws_dup(); ws_push(100); ws_sub();
    ws_jz(f_finished_1);

    ws_dup();
    ws_push(100); ws_add();
	    ws_push(1);
    ws_store();

    ws_push(1); ws_add();
    ws_jump(f_start_1);

    ws_label(f_finished_1);
    ws_drop();
}

//debug_printstack
//debug_printheap

{
/*
    for(i=2; i<11; i++)
	for(j=2; j<100; j++)
	    memory[100+i*j] = 0;
 */

    ws_push(2);
    //for loop
    ws_label(f_start_2);
    ws_dup(); ws_push(11); ws_sub();
    ws_jz(f_finished_2);



	    ws_push(2);
	    //for loop
	    ws_label(f_start_3);
    ws_dup(); ws_push(100); ws_sub();
	    ws_jz(f_finished_3);

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

	    ws_jump(f_start_3);
	    ws_label(f_finished_3);
	    ws_drop();


    ws_push(1); ws_add();

    ws_jump(f_start_2);
    ws_label(f_finished_2);
    ws_drop();
}

{
    ws_push(2);
    //for loop
    ws_label(f_start_4);
    ws_dup(); ws_push(100); ws_sub();
    ws_jz(f_finished_4);

    ws_dup();
    ws_push(100); ws_add();
    ws_fetch();
    ws_jz(nodraw);

    ws_dup();
    ws_outn();

    ws_pushs(" ");
    ws_call(prints);

    ws_label(nodraw);

    ws_push(1); ws_add();
    ws_jump(f_start_4);

    ws_label(f_finished_4);
    ws_drop();
}

ws_push(0);
ws_call(printsln);

ws_exit();

ws_label(prints);

ws_dup();
ws_jz(prints_end);
ws_outc();
ws_jump(prints);

ws_label(prints_end);
ws_drop();
ws_return();

ws_label(printsln);
ws_call(prints);
ws_push(10);
ws_outc();
ws_return();

ws_trailer
