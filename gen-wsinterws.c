
#define ATOMISE_LABELS
#define xVERBOSE

#include "ws_gencode.h"
main() {
/*
options

print_trace      print each instruction when it is run

*/

#undef ws_jnz
#define ws_jnz ws_jzn
#define ws_jpz ws_jzp

ws_call(main);
ws_exit();



#include "io.h"
#include "memory.h"

/*

//global variables
// var 0-19 are reserved for local vars
// var 20: start of code
// var 21: end of code
// var 22: start of byte_compiled_code
// var 23: end of byte_compiled_code
// var 24: start of instruction_compiled_code
// var 25: end of instruction_compiled_code
// var 26: start of stack
// var 27: end of stack ;not included
// var 28: start of heap
// var 29: end of heap ;not included
// var 40-60 are reserved for local vars
// var 100-... is reserved for big data
*/

ws_label(main);
ws_call(run);
ws_return();

ws_label(run);
ws_call(intro);

//pushs "beginning reading"
//call printsln


// read program
ws_push(20);
ws_push(100);
ws_store();

ws_push(20);
ws_fetch();

ws_call(read_program);

ws_push(21);
ws_swap();
ws_store();

ws_push(22);
ws_push(21);
ws_fetch();
ws_push(10);
ws_add();
ws_store();

ws_pushs("-- WS Interpreter WS -------------------------------------------");
ws_call(printsln);

//pushs "compiling to byte code"
//call printsln

ws_push(20);
ws_fetch();
ws_push(21);
ws_fetch();
ws_push(22);
ws_fetch();

ws_call(compile_byte_code);

ws_push(23);
ws_swap();
ws_store();

ws_push(24);
ws_push(23);
ws_fetch();
ws_push(10);
ws_add();
ws_store();

//pushs "compiling to instruction code"
//call printsln



ws_push(22);
ws_fetch();
ws_push(23);
ws_fetch();
ws_push(24);
ws_fetch();

ws_call(compile_instruction_code);

ws_push(25);
ws_swap();
ws_store();

//pushs "compiling labels"
//call printsln

ws_call(compile_labels);



//pushs "creating stack"
//call printsln

ws_push(26);
ws_push(25);
ws_fetch();
ws_push(10);
ws_add();
ws_store();//stack start

ws_push(27);
ws_push(26);
ws_fetch();
ws_store();//stack end ;same as start

ws_push(26);
ws_fetch();
ws_push(10);
ws_call(mem_zero);

//pushs "creating heap"
//call printsln

ws_push(28);
ws_push(27);
ws_fetch();
ws_push(10);
ws_add();
ws_store();//heap start

ws_push(29);
ws_push(28);
ws_fetch();
ws_store();// heap end

ws_push(28);
ws_fetch();
ws_push(10);
ws_call(mem_zero);

//pushs "starting to interpret"
//call printsln

#ifdef print_compilation
// debug_printheap
#endif

ws_call(interpret);

ws_exit();





// the label data has the number of the label it refers to at the beginning
// this funktion replaces it by the actual adress of the data
// parameter
// return
// variables
ws_label(compile_labels);

ws_push(24);
ws_fetch();

ws_label(compile_labels_loop);

ws_dup();
ws_push(25);
ws_fetch();
ws_sub();
ws_jpz(compile_labels_end_pop);

ws_dup();
ws_fetch();// got the type

ws_dup(); ws_push(8); ws_sub();
ws_jz(compile_labels_is_label);

ws_dup(); ws_push(9); ws_sub();
ws_jz(compile_labels_is_label);

ws_dup(); ws_push(10); ws_sub();
ws_jz(compile_labels_is_label);

ws_dup(); ws_push(11); ws_sub();
ws_jz(compile_labels_is_label);


ws_drop();

ws_push(2); ws_add();// ++ pointer

ws_jump(compile_labels_loop);

ws_label(compile_labels_is_label);
ws_drop();

ws_dup();
ws_push(1); ws_add();

ws_dup();
ws_fetch();

ws_call(get_label_ip);
ws_store();


ws_push(2); ws_add();// ++ pointer

ws_jump(compile_labels_loop);


ws_label(compile_labels_end_pop);
ws_drop();

ws_return();





// find label
// parameter
//  [1] label
// return
//  [1] ip
// variable (are saved)
//  [1] label

ws_label(get_label_ip);

ws_push(1);
ws_fetch();
// save 1

ws_swap();

ws_push(1);
ws_swap();
ws_store();


ws_push(24);
ws_fetch();

ws_label(get_label_ip_loop);
ws_dup();
ws_push(25);
ws_fetch();
ws_sub();
ws_jpz(get_label_ip_end_pop);

ws_dup();
ws_fetch();// got the type

ws_dup(); ws_push(7); ws_sub();
ws_jz(get_label_ip_is_label);


ws_drop();

ws_push(2); ws_add();// ++ pointer

ws_jump(get_label_ip_loop);

ws_label(get_label_ip_is_label);
ws_drop();

ws_dup();
ws_push(1); ws_add();
ws_fetch();
ws_push(1);
ws_fetch();
ws_sub();
ws_jz(get_label_ip_found_label);

ws_push(2); ws_add();// ++ pointer
ws_jump(get_label_ip_loop);



ws_label(get_label_ip_found_label);
ws_swap();
ws_push(1);
ws_swap();
ws_store();
//restore [1]

ws_return();



ws_label(get_label_ip_end_pop);
ws_drop();

ws_pushs("error: label not found: ");
ws_call(prints);
ws_push(1); ws_fetch();
ws_outn();
ws_push(0);
ws_call(printsln);

ws_push(1);
ws_store();
//restore [1]

ws_push(0);

ws_exit();
//ret









// parameter
// [1] new item
// value
ws_label(stack_push_back);
ws_push(27);
ws_push(27); ws_fetch();//old stack end
ws_push(1); ws_add();// new stack end
ws_store();
    // just set new and and let the memory arrange

ws_call(memory_arrange);

ws_push(27); ws_fetch();
ws_push(1); ws_sub();//ptr to back
ws_swap();
ws_store();
    // save new item

ws_return();


// parameter
// [1] new item
// value
ws_label(stack_push_front);
ws_push(26);
ws_push(26); ws_fetch();//old stack start
ws_push(1); ws_sub();// new stack start
ws_store();
    // just set new and and let the memory arrange

ws_call(memory_arrange);

ws_push(26); ws_fetch();
ws_swap();
ws_store();
    // save new item

ws_return();







// parameter
// return
// [1] value
ws_label(stack_pop_back);
ws_push(26); ws_fetch();//start of stack
ws_push(27); ws_fetch();//end of stack
ws_sub();
ws_jz(stack_pop_back_error_empty);

ws_push(27); //end of stack
ws_fetch();
ws_push(1);
ws_sub();// last element
ws_fetch();


ws_push(27); //end of stack
ws_fetch();
ws_push(1);
ws_sub();// last element
ws_push(0);
ws_store();
        // erase old element

ws_push(27);
ws_push(27); //end of stack
ws_fetch();
ws_push(1);
ws_sub();
ws_store();

ws_call(memory_arrange);
ws_return();


ws_label(stack_pop_back_error_empty);
ws_pushs("error: stack_pop_back and stack empty");
ws_call(printsln);
ws_push(-1);
ws_return();

// parameter
// return
// [1] value
ws_label(stack_pop_front);
ws_push(26); //start of stack
ws_fetch();
ws_push(27); //end of stack
ws_fetch();
ws_sub();
ws_jz(stack_pop_front_error_empty);

ws_push(26); //start of stack
ws_fetch();// first element
ws_fetch();

ws_push(26); //start of stack
ws_fetch();// first element
ws_push(0);
ws_store();
        // erase old element

ws_push(26);
ws_push(26); //end of stack
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_call(memory_arrange);
ws_return();


ws_label(stack_pop_front_error_empty);
ws_pushs("error: stack_pop_front and stack empty");
ws_call(printsln);
ws_push(-1);
ws_return();


// makes sure the address is available in heap (resizing it if necessary)
// parameter
//  [1] address
ws_label(heap_extend_to_address);
ws_dup();
ws_jn(heap_extend_to_address_done_pop);//error !! negative address

//      doub
ws_push(28); ws_fetch();//heap start
ws_add();
ws_push(29); ws_fetch();//heap end
ws_sub();
ws_push(1); ws_add();
    // add xxx allocate xxx byte additional for seldom realloc
ws_dup();
ws_jnz(heap_extend_to_address_done_pop);
//    jumpn heap_extend_to_address_done_pop
//    doub
//    jumpz heap_extend_to_address_done_pop

ws_dup();
ws_push(29);
ws_fetch();
ws_swap();
ws_call(mem_zero);//zero the new memory

ws_dup();
ws_push(29);
ws_fetch();
ws_add();
ws_push(29);
ws_swap();
ws_store();//store new memory end

ws_label(heap_extend_to_address_done_pop);
ws_drop();
ws_return();



// parameter
//  [2] address
//  [1] value
// return
ws_label(heap_store);


//    pushs "heap_store addr "
//    call prints
//    swap
//    doub
//    outn
//    pushs " val "
//    call prints
//    swap
//    doub
//    outn


//    debug_printheap

ws_swap();
ws_dup();
ws_jn(heap_store_negative_address);

ws_dup();

//    debug_printstack
ws_call(heap_extend_to_address);
//    debug_printstack
//    debug_printheap

//    debug_printstack

ws_push(28); //heap start
ws_fetch();
ws_add();

ws_swap();

//    debug_printstack

ws_store();
//    debug_printheap
//    call interpreter_debug_printheap

ws_return();

ws_label(heap_store_negative_address);
ws_pushs("error: heap store, negative address: ");
ws_call(printsln);
ws_outn();
ws_drop();
ws_return();


// parameter
//  [1] address
// return
//  [1] value
ws_label(heap_retrive);

ws_dup();
ws_jn(heap_retrive_address_negative);

ws_push(28); //heap start
ws_fetch();
ws_add();

ws_dup();
ws_push(29); //heap end
ws_fetch();
ws_swap();
ws_sub();
ws_jnz(heap_retrive_address_to_big);
//    debug_printstack
ws_fetch();
//    debug_printstack

ws_return();

ws_label(heap_retrive_address_negative);
ws_pushs("error: heap retrive: address negative: ");
ws_call(prints);
ws_outn();
ws_push(0);
ws_call(printsln);
ws_push(0);
ws_return();

ws_label(heap_retrive_address_to_big);
ws_pushs("error: heap retrive: address too big: ");
ws_call(prints);
ws_outn();
ws_push(0);
ws_call(printsln);
ws_push(0);
ws_return();








// rearranges the stack and heap
// after calling this you can be sure of:
// code_inst_end < stack_start < stack_end < heap_start < heap_end
// 10 < code_inst_end - stack_start < 100
// 10 < stack_end - heap_start < 100
//variables (saved)
// [1] stack_start
// [2] stack_end
// [3] stack_length
// [4] heap_start
// [5] heap_end
// [6] heap_length

ws_label(memory_arrange);
ws_push(1);
ws_fetch();
ws_push(2);
ws_fetch();
ws_push(3);
ws_fetch();
ws_push(4);
ws_fetch();
ws_push(5);
ws_fetch();
ws_push(6);
ws_fetch();
    // save the vars

ws_push(3);
ws_push(27);
ws_fetch();
ws_push(26);
ws_fetch();
ws_sub();
ws_store();
    // get stack length

ws_push(6);
ws_push(29);
ws_fetch();
ws_push(28);
ws_fetch();
ws_sub();
ws_store();
    // get heap length

    //code_inst_end + 10 > stack_start
    //code_inst_end + 100 < stack_start
    //     stack_start = code_inst_end + 20
    //else
    //     stack_start = old stack_start

    // stack_end = stack_start + stack_length

ws_push(26);
ws_fetch();// stack_start
ws_push(25);
ws_fetch();// code_inst_end
ws_push(10);
ws_add();
ws_sub();
ws_jn(memory_arrange_set_stack_start);

ws_push(25); // code_inst_end
ws_fetch();
ws_push(100);
ws_add();
ws_push(26); // stack start
ws_fetch();
ws_sub();
ws_jn(memory_arrange_set_stack_start);

ws_jump(memory_arrange_old_stack_start);

ws_label(memory_arrange_old_stack_start);
ws_push(1);
ws_push(26);
ws_fetch();
ws_store();
ws_jump(memory_arrange_calc_heap);

ws_label(memory_arrange_set_stack_start);
ws_push(1);
ws_push(25);
ws_fetch();
ws_push(20);
ws_add();
ws_store();
ws_jump(memory_arrange_calc_heap);


ws_label(memory_arrange_calc_heap);
ws_push(2); //end
ws_push(1);
ws_fetch();//start
ws_push(3);
ws_fetch();//length
ws_add();
ws_store();


    //stack_end + 10 > heap_start
    //stack_end + 100 < heap_start
    //     heap_start = stack_end + 20
    //else
    //     heap_start = old heap_start

ws_push(28);
ws_fetch();// heap_start
ws_push(27);
ws_fetch();// stack_end
ws_push(10);
ws_add();
ws_sub();
ws_jn(memory_arrange_set_heap_start);

ws_push(27); // stack_end
ws_fetch();
ws_push(100);
ws_add();
ws_push(28); // heap start
ws_fetch();
ws_sub();
ws_jn(memory_arrange_set_heap_start);

ws_jump(memory_arrange_old_heap_start);

ws_label(memory_arrange_old_heap_start);
ws_push(4);
ws_push(28);
ws_fetch();
ws_store();
ws_jump(memory_arrange_calc_heap_end);

ws_label(memory_arrange_set_heap_start);
ws_push(4);
ws_push(27);
ws_fetch();
ws_push(20);
ws_add();
ws_store();
ws_jump(memory_arrange_calc_heap_end);


ws_label(memory_arrange_calc_heap_end);
ws_push(5); //end
ws_push(4);
ws_fetch();//start
ws_push(6);
ws_fetch();//length
ws_add();
ws_store();

// save [1]-[3]
ws_push(1); ws_fetch();
ws_push(2); ws_fetch();
ws_push(3); ws_fetch();

    // move heap to heap_start
ws_push(26); // stack start
ws_fetch();//source
ws_push(1); // new stack start
ws_fetch();//dest
ws_push(3);
ws_fetch();// length
ws_call(mem_move);

// restore [1]-[3]
ws_push(3); ws_swap(); ws_store();
ws_push(2); ws_swap(); ws_store();
ws_push(1); ws_swap(); ws_store();


ws_push(26);
ws_push(1);
ws_fetch();
ws_store();

ws_push(27);
ws_push(2);
ws_fetch();
ws_store();

// save [1]-[3]
ws_push(1); ws_fetch();
ws_push(2); ws_fetch();
ws_push(3); ws_fetch();

    // move stack to stack_start
ws_push(28); // stack start
ws_fetch();//source
ws_push(4); // new stack start
ws_fetch();//dest
ws_push(6);
ws_fetch();// length
ws_call(mem_move);

// restore [1]-[3]
ws_push(3); ws_swap(); ws_store();
ws_push(2); ws_swap(); ws_store();
ws_push(1); ws_swap(); ws_store();

ws_push(28);
ws_push(4);
ws_fetch();
ws_store();

ws_push(29);
ws_push(5);
ws_fetch();
ws_store();




ws_push(6);
ws_swap();
ws_store();
ws_push(5);
ws_swap();
ws_store();
ws_push(4);
ws_swap();
ws_store();
ws_push(3);
ws_swap();
ws_store();
ws_push(2);
ws_swap();
ws_store();
ws_push(1);
ws_swap();
ws_store();
    // restore the vars
ws_return();




// parameter:
// return
//
// variables
// [1] inst code pointer
// [2] inst code
// [3] data code
ws_label(interpret);
ws_push(1);
ws_push(24);
ws_fetch();
ws_store();

//    debug_printheap

ws_label(interpret_start);

ws_jump(interpret_next);

ws_label(interpret_next_pop);
ws_drop();

ws_label(interpret_next);

//    pushs " "
//    call printsln

//    retrive 1
//  outn

ws_push(1); ws_fetch();
ws_jn(interpret_end);

ws_push(1); ws_fetch();
ws_push(25); ws_fetch();
ws_sub();
ws_jp(interpret_end);


ws_push(1);
ws_fetch();
ws_fetch();
    // get inst

ws_dup();
ws_push(2);
ws_swap();
ws_store();

ws_push(3);
ws_push(1); ws_fetch();
ws_push(1); ws_add();
ws_fetch();
ws_store();// get data

ws_push(1);
ws_push(1); ws_fetch();
ws_push(2); ws_add();
ws_store();
    // next ip


#ifdef trace_stack
ws_call(interpreter_debug_printstack);
#endif

#ifdef trace_heap
ws_call(interpreter_debug_printheap);
#endif

ws_dup(); ws_push(1); ws_sub();
ws_jz(interpret_push);

ws_dup(); ws_push(2); ws_sub();
ws_jz(interpret_pop);

ws_dup(); ws_push(3); ws_sub();
ws_jz(interpret_doub);

ws_dup(); ws_push(4); ws_sub();
ws_jz(interpret_swap);

ws_dup(); ws_push(5); ws_sub();
ws_jz(interpret_store);

ws_dup(); ws_push(6); ws_sub();
ws_jz(interpret_retrive);

ws_dup(); ws_push(7); ws_sub();
ws_jz(interpret_label);

ws_dup(); ws_push(8); ws_sub();
ws_jz(interpret_call);

ws_dup(); ws_push(9); ws_sub();
ws_jz(interpret_jump);

ws_dup(); ws_push(10); ws_sub();
ws_jz(interpret_jumpz);

ws_dup(); ws_push(11); ws_sub();
ws_jz(interpret_jumpn);

ws_dup(); ws_push(12); ws_sub();
ws_jz(interpret_ret);

ws_dup(); ws_push(13); ws_sub();
ws_jz(interpret_exit);

ws_dup(); ws_push(14); ws_sub();
ws_jz(interpret_add);

ws_dup(); ws_push(15); ws_sub();
ws_jz(interpret_sub);

ws_dup(); ws_push(16); ws_sub();
ws_jz(interpret_mul);

ws_dup(); ws_push(17); ws_sub();
ws_jz(interpret_div);

ws_dup(); ws_push(18); ws_sub();
ws_jz(interpret_mod);

ws_dup(); ws_push(19); ws_sub();
ws_jz(interpret_outc);

ws_dup(); ws_push(20); ws_sub();
ws_jz(interpret_outn);

ws_dup(); ws_push(21); ws_sub();
ws_jz(interpret_inc);

ws_dup(); ws_push(22); ws_sub();
ws_jz(interpret_inn);

ws_dup(); ws_push(23); ws_sub();
ws_jz(interpret_debugprintstack);

ws_dup(); ws_push(24); ws_sub();
ws_jz(interpret_debugprintheap);

ws_jump(interpret_parse_error);

ws_label(interpret_parse_error);
    //debug_printstack
ws_pushs("error: unknown instruction code ");
ws_call(prints);
ws_outn();
ws_push(0);
ws_call(printsln);

ws_jump(interpret_end);



ws_label(interpret_push);
ws_push(3); ws_fetch();

#ifdef print_trace
ws_pushs("push ");
ws_call(prints);
ws_dup();
ws_outn();
ws_push(0);
ws_call(printsln);
#endif

ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_pop);
#ifdef print_trace
ws_pushs("pop");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_jump(interpret_next_pop);

ws_label(interpret_label);
#ifdef print_trace
ws_pushs("label");
ws_call(printsln);
#endif

ws_jump(interpret_next_pop);

ws_label(interpret_doub);
#ifdef print_trace
ws_pushs("doub");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_dup();
ws_call(stack_push_back);
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_swap);
#ifdef print_trace
ws_pushs("swap");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_call(stack_push_back);
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_store);
#ifdef print_trace
ws_pushs("store");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_call(heap_store);
ws_jump(interpret_next_pop);

ws_label(interpret_retrive);
#ifdef print_trace
ws_pushs("retrive");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(heap_retrive);
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_call);
#ifdef print_trace
ws_pushs("call");
ws_call(printsln);
#endif

ws_push(1);
ws_fetch();// next ip to jump over data
ws_call(stack_push_front);

ws_push(3);
ws_fetch();

ws_push(1);
ws_swap();
ws_store();

ws_jump(interpret_next_pop);

ws_label(interpret_jump);
#ifdef print_trace
ws_pushs("jump");
ws_call(printsln);
#endif

ws_push(3);
ws_fetch();

ws_push(1);
ws_swap();
ws_store();
ws_jump(interpret_next_pop);

ws_label(interpret_jumpz);
#ifdef print_trace
ws_pushs("jumpz");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_jz(interpret_jump);
ws_jump(interpret_next_pop);

ws_label(interpret_jumpn);
#ifdef print_trace
ws_pushs("jumpn");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_jn(interpret_jump);
ws_jump(interpret_next_pop);

ws_label(interpret_ret);
#ifdef print_trace
ws_pushs("ret");
ws_call(printsln);
#endif

ws_call(stack_pop_front);

ws_push(1);
ws_swap();
ws_store();
ws_jump(interpret_next_pop);

ws_label(interpret_exit);
#ifdef print_trace
ws_pushs("exit");
ws_call(printsln);
#endif

ws_exit();
ws_jump(interpret_next_pop);

ws_label(interpret_add);
#ifdef print_trace
ws_pushs("add");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_add();
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_sub);
#ifdef print_trace
ws_pushs("sub");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_sub();
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_mul);
#ifdef print_trace
ws_pushs("mul");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_mul();
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_div);
#ifdef print_trace
ws_pushs("div");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_div();
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_mod);
#ifdef print_trace
ws_pushs("mod");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_call(stack_pop_back);
ws_swap();
ws_mod();
ws_call(stack_push_back);
ws_jump(interpret_next_pop);

ws_label(interpret_outc);
#ifdef print_trace
ws_pushs("outc");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_outc();
ws_jump(interpret_next_pop);

ws_label(interpret_outn);
#ifdef print_trace
ws_pushs("outn");
ws_call(printsln);
#endif

ws_call(stack_pop_back);
ws_outn();
ws_jump(interpret_next_pop);

ws_label(interpret_inc);
#ifdef print_trace
ws_pushs("inc");
ws_call(printsln);
#endif

ws_call(stack_pop_back);

ws_push(8);
ws_readc();
ws_push(8); ws_fetch();

ws_call(heap_store);

ws_jump(interpret_next_pop);

ws_label(interpret_inn);
#ifdef print_trace
ws_pushs("inn");
ws_call(printsln);
#endif

ws_call(stack_pop_back);

ws_push(8);
ws_readn();
ws_push(8); ws_fetch();

ws_call(heap_store);
ws_jump(interpret_next_pop);

ws_label(interpret_debugprintstack);
#ifdef print_trace
ws_pushs("debug_printstack");
ws_call(printsln);
#endif

ws_call(interpreter_debug_printstack);
ws_jump(interpret_next_pop);

ws_label(interpret_debugprintheap);
#ifdef print_trace
ws_pushs("debug_printheap");
ws_call(printsln);
#endif

ws_call(interpreter_debug_printheap);
ws_jump(interpret_next_pop);



ws_label(interpret_end);
ws_return();





ws_label(interpreter_debug_printheap);
ws_pushs("Heap: [");
ws_call(prints);

ws_push(28);
ws_fetch();// heap start

ws_label(interpret_debugprintheap_loop);
ws_dup();
ws_push(29);
ws_fetch();// heap end
ws_sub();
ws_jz(interpret_debugprintheap_loop_end);

    //???
    //    doub
    //outN

ws_dup();
ws_fetch();

ws_outn();
ws_pushs(",");
ws_call(prints);

ws_push(1); ws_add();
ws_jump(interpret_debugprintheap_loop);

ws_label(interpret_debugprintheap_loop_end);

ws_drop();

ws_pushs("]");
ws_call(printsln);
ws_return();







ws_label(interpreter_debug_printstack);
ws_pushs("Stack: [");
ws_call(prints);

ws_push(26);
ws_fetch();// stack start

ws_label(interpret_debugprintstack_loop);
ws_dup();
ws_push(27);
ws_fetch();// stack end
ws_sub();
ws_jz(interpret_debugprintstack_loop_end);

ws_dup();
ws_fetch();

ws_outn();
ws_pushs(",");
ws_call(prints);

ws_push(1); ws_add();
ws_jump(interpret_debugprintstack_loop);

ws_label(interpret_debugprintstack_loop_end);

ws_drop();

ws_pushs("]");
ws_call(printsln);
ws_return();




ws_label(intro);
ws_pushs("WhiteSpace interpreter written in Whitespace");
ws_call(printsln);
ws_pushs("Made by Oliver Burghard Smarty21@gmx.net");
ws_call(printsln);
ws_pushs("in his free time for your and his joy");
ws_call(printsln);
ws_pushs("good time and join me to get Whitespace ready for business");
ws_call(printsln);
ws_pushs("For any other information dial 1-900-WHITESPACE");
ws_call(printsln);
ws_pushs("Or get soon info at www.WHITESPACE-WANTS-TO-BE-TAKEN-SERIOUS.org");
ws_call(printsln);
ws_pushs("please enter the program and terminate via 3xEnter,'quit',3xEnter");
ws_call(printsln);
ws_return();






// parameter:
// [1] start byte code
// [2] end byte code
// [3] start inst code
// return
// [1] end inst code
//
// variables
// [1] start byte code
// [2] end byte code
// [3] start inst code
// [4] byte code pointer
// [5] inst code pointer
ws_label(compile_instruction_code);

ws_push(3);
ws_swap();
ws_store();

ws_push(2);
ws_swap();
ws_store();

ws_push(1);
ws_swap();
ws_store();


ws_push(4);
ws_push(1);
ws_fetch();
ws_store();

ws_push(5);
ws_push(3);
ws_fetch();
ws_store();
// all local variables set

ws_label(compile_inst_code_loop_restart);

// p < e - 2
// p - e + 2 < 0
ws_push(4);
ws_fetch();
ws_push(2);
ws_fetch();
ws_sub();
ws_push(3); // end - 3 because at least 3 chars should be left
ws_add();

// finished, all data done
ws_jn(compile_inst_code_continue);
ws_jump(compile_inst_code_end);


ws_label(compile_inst_code_continue);

ws_push(4);
ws_fetch();

ws_call(getinst);
// now we have type, size, value on stack


//increment code pointer
ws_push(4);
ws_fetch();
ws_add();
ws_push(4);
ws_swap();
ws_store();

// save inst
ws_push(5);
ws_fetch();
ws_swap();
ws_store();

//increment compiled code pointer
ws_push(5);
ws_push(5);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

// save number
ws_push(5);
ws_fetch();
ws_swap();
ws_store();

//increment compiled code pointer
ws_push(5);
ws_push(5);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_jump(compile_inst_code_loop_restart);


ws_label(compile_inst_code_end);

// get inst code pointer
// return it
ws_push(5);
ws_fetch();

ws_return();




// get instruction
// parameter
// [1] address of instruction start
// return
// [1] source legnth
// [2] instruction
// [3] number
// variables
// [11-15] instructions copied (1-5)
// [16] number (only if has number)
// [17] number length (only if has number)
// [18] address of instruction start

ws_label(getinst);
ws_push(18);
ws_swap();
ws_store();

ws_push(18);
ws_fetch();

ws_dup();
ws_fetch();
ws_push(11);
ws_swap();
ws_store();

ws_push(1);
ws_add();

ws_dup();
ws_fetch();
ws_push(12);
ws_swap();
ws_store();

ws_push(1);
ws_add();

ws_dup();
ws_fetch();
ws_push(13);
ws_swap();
ws_store();

ws_push(1);
ws_add();

ws_dup();
ws_fetch();
ws_push(14);
ws_swap();
ws_store();

ws_push(1);
ws_add();

ws_fetch();
ws_push(15);
ws_swap();
ws_store();




ws_push(11);
ws_fetch();
ws_push(3);
ws_mul();
ws_push(12);
ws_fetch();
ws_add();
// now we have [1]*3+[2]
ws_dup();
ws_jz(getinst_push);

ws_push(3);
ws_mul();
ws_push(13);
ws_fetch();
ws_add();
// now we have [1]*9+[2]*3+[3]

ws_dup(); ws_push(8); ws_sub();
ws_jz(getinst_pop);

ws_dup(); ws_push(6); ws_sub();
ws_jz(getinst_doub);

ws_dup(); ws_push(7); ws_sub();
ws_jz(getinst_swap);

ws_dup(); ws_push(12); ws_sub();
ws_jz(getinst_store);

ws_dup(); ws_push(13); ws_sub();
ws_jz(getinst_retrive);

ws_dup(); ws_push(18); ws_sub();
ws_jz(getinst_label);

ws_dup(); ws_push(19); ws_sub();
ws_jz(getinst_call);

ws_dup(); ws_push(20); ws_sub();
ws_jz(getinst_jump);

ws_dup(); ws_push(21); ws_sub();
ws_jz(getinst_jumpz);

ws_dup(); ws_push(22); ws_sub();
ws_jz(getinst_jumpn);

ws_dup(); ws_push(23); ws_sub();
ws_jz(getinst_ret);

ws_dup(); ws_push(26); ws_sub();
ws_jz(getinst_exit);

ws_push(3);
ws_mul();
ws_push(14);
ws_fetch();
ws_add();
// now we have [1]*27+[2]*9+[3]*3+[4]

ws_dup(); ws_push(27); ws_sub();
ws_jz(getinst_add);

ws_dup(); ws_push(28); ws_sub();
ws_jz(getinst_sub);

ws_dup(); ws_push(29); ws_sub();
ws_jz(getinst_mul);

ws_dup(); ws_push(30); ws_sub();
ws_jz(getinst_div);

ws_dup(); ws_push(31); ws_sub();
ws_jz(getinst_mod);

ws_dup(); ws_push(45); ws_sub();
ws_jz(getinst_outc);

ws_dup(); ws_push(46); ws_sub();
ws_jz(getinst_outn);

ws_dup(); ws_push(48); ws_sub();
ws_jz(getinst_inc);

ws_dup(); ws_push(49); ws_sub();
ws_jz(getinst_inn);

ws_push(3);
ws_mul();
ws_push(15);
ws_fetch();
ws_add();
// now we have [1]*81+[2]*27+[3]*9+[4]*3+[5]

ws_dup(); ws_push(216); ws_sub();
ws_jz(getinst_debug_printstack);

ws_dup(); ws_push(217); ws_sub();
ws_jz(getinst_debug_printheap);

ws_jump(getinst_not_known);

ws_label(getinst_not_known);
ws_pushs("error, type unknown: ");
ws_call(printsln);
ws_push(11); ws_fetch();
ws_outn();
ws_push(12); ws_fetch();
ws_outn();
ws_push(13); ws_fetch();
ws_outn();
ws_push(14); ws_fetch();
ws_outn();
ws_push(15); ws_fetch();
ws_outn();
ws_pushs(" ");
ws_call(prints);
ws_outn();
ws_push(0);
ws_call(printsln);
// debug_printheap
// debug_printstack

ws_push(-1);
ws_push(100);
ws_return();


ws_label(getinst_push);
ws_drop();
ws_push(18);
ws_fetch();//code start
ws_push(2); // inst length
ws_add();
ws_call(scan_number);
ws_push(16);
ws_swap();
ws_store();
ws_push(17);
ws_swap();
ws_store();

ws_push(16);
ws_fetch();
// number
ws_push(1); //inst number
ws_push(17);
ws_fetch();//number length
ws_push(2); //inst length
ws_add();
ws_return();

ws_label(getinst_pop);
ws_drop();
ws_push(0);
ws_push(2); //inst number
ws_push(3);
ws_return();

ws_label(getinst_doub);
ws_drop();
ws_push(0);
ws_push(3); //inst number
ws_push(3);
ws_return();

ws_label(getinst_swap);
ws_drop();
ws_push(0);
ws_push(4); //inst number
ws_push(3);
ws_return();

ws_label(getinst_store);
ws_drop();
ws_push(0);
ws_push(5); //inst number
ws_push(3);
ws_return();

ws_label(getinst_retrive);
ws_drop();
ws_push(0);
ws_push(6); //inst number
ws_push(3);
ws_return();

ws_label(getinst_label);
ws_drop();
ws_push(18);
ws_fetch();//code start
ws_push(3); // inst length
ws_add();
ws_call(scan_label);
ws_push(16);
ws_swap();
ws_store();
ws_push(17);
ws_swap();
ws_store();

ws_push(16);
ws_fetch();
// number
ws_push(7); //inst number
ws_push(17);
ws_fetch();//number length
ws_push(3); //inst length
ws_add();
ws_return();

ws_label(getinst_call);
ws_drop();
ws_push(18);
ws_fetch();//code start
ws_push(3); // inst length
ws_add();
ws_call(scan_label);
ws_push(16);
ws_swap();
ws_store();
ws_push(17);
ws_swap();
ws_store();

ws_push(16);
ws_fetch();
// number
ws_push(8); //inst number
ws_push(17);
ws_fetch();//number length
ws_push(3); //inst length
ws_add();
ws_return();

ws_label(getinst_jump);
ws_drop();
ws_push(18);
ws_fetch();//code start
ws_push(3); // inst length
ws_add();
ws_call(scan_label);
ws_push(16);
ws_swap();
ws_store();
ws_push(17);
ws_swap();
ws_store();

ws_push(16);
ws_fetch();
// number
ws_push(9); //inst number
ws_push(17);
ws_fetch();//number length
ws_push(3); //inst length
ws_add();
ws_return();

ws_label(getinst_jumpz);
ws_drop();
ws_push(18);
ws_fetch();//code start
ws_push(3); // inst length
ws_add();
ws_call(scan_label);
ws_push(16);
ws_swap();
ws_store();
ws_push(17);
ws_swap();
ws_store();

ws_push(16);
ws_fetch();
// number
ws_push(10); //inst number
ws_push(17);
ws_fetch();//number length
ws_push(3); //inst length
ws_add();
ws_return();

ws_label(getinst_jumpn);
ws_drop();
ws_push(18);
ws_fetch();//code start
ws_push(3); // inst length
ws_add();
ws_call(scan_label);
ws_push(16);
ws_swap();
ws_store();
ws_push(17);
ws_swap();
ws_store();

ws_push(16);
ws_fetch();
// number
ws_push(11); //inst number
ws_push(17);
ws_fetch();//number length
ws_push(3); //inst length
ws_add();
ws_return();

ws_label(getinst_ret);
ws_drop();
ws_push(0);
ws_push(12); //inst number
ws_push(3);
ws_return();

ws_label(getinst_exit);
ws_drop();
ws_push(0);
ws_push(13); //inst number
ws_push(3);
ws_return();

ws_label(getinst_add);
ws_drop();
ws_push(0);
ws_push(14); //inst number
ws_push(4);
ws_return();

ws_label(getinst_sub);
ws_drop();
ws_push(0);
ws_push(15); //inst number
ws_push(4);
ws_return();

ws_label(getinst_mul);
ws_drop();
ws_push(0);
ws_push(16); //inst number
ws_push(4);
ws_return();

ws_label(getinst_div);
ws_drop();
ws_push(0);
ws_push(17); //inst number
ws_push(4);
ws_return();

ws_label(getinst_mod);
ws_drop();
ws_push(0);
ws_push(18); //inst number
ws_push(4);
ws_return();

ws_label(getinst_outc);
ws_drop();
ws_push(0);
ws_push(19); //inst number
ws_push(4);
ws_return();

ws_label(getinst_outn);
ws_drop();
ws_push(0);
ws_push(20); //inst number
ws_push(4);
ws_return();

ws_label(getinst_inc);
ws_drop();
ws_push(0);
ws_push(21); //inst number
ws_push(4);
ws_return();

ws_label(getinst_inn);
ws_drop();
ws_push(0);
ws_push(22); //inst number
ws_push(4);
ws_return();

ws_label(getinst_debug_printstack);
ws_drop();
ws_push(0);
ws_push(23); //inst number
ws_push(5);
ws_return();

ws_label(getinst_debug_printheap);
ws_drop();
ws_push(0);
ws_push(24); //inst number
ws_push(5);
ws_return();



//push 0 ;no number
//push 2 ;type
//push 1 ;length
//ret


//01-0Push i) ls = "aa" ++ " " ++ (integerToString i)
//02-8Pop) ls = "acc"
//03-6Doub) ls  = "aca"
//04-7Swap) ls  = "acb"
//05-12Store) ls = "bba"
//06-13Retrive) ls = "bbb"
//07-9Label l) ls = "caa" ++ " " ++ (labelToString ls l)
//08-19Call l) ls = "cab" ++ " " ++ (labelToString ls l)
//09-20Jump l) ls = "cac" ++ " " ++ (labelToString ls l)
//10-21JumpZ l) ls = "cba" ++ " " ++ (labelToString ls l)
//11-22JumpN l) ls = "cbb" ++ " " ++ (labelToString ls l)
//12-23Ret) ls = "cbc"
//13-26Exit) ls = "ccc"
//14-27Add) ls = "baaa"
//15-28Sub) ls = "baab"
//16-29Mul) ls = "baac"
//17-30Div) ls = "baba"
//18-31Mod) ls = "babb"
//19-45OutC) ls = "bcaa"
//20-46OutN) ls = "bcab"
//21-47InC) ls = "bcba"
//22-48InN) ls = "bcbb"
//23-60Debug_PrintStack) ls = "ccaaa"
//24-61Debug_PrintHeap) ls = "ccaab"



// scan number
//   a = 0, b = 1, c = ende
//   first a,b is sign
//   next a,b are binary numbers
//   first c is terminator
// parameter
// [1] start of number
// return
// [1] number
// [2] length (including terminating 'c')
// variables
// [40] start pointer
// [41] number
// [42] sign
// [43] actual pointer

ws_label(scan_number);
ws_push(42);
ws_push(1);
ws_store();

ws_push(41);
ws_push(0);
ws_store();

ws_push(40);
ws_swap();
ws_store();

ws_push(43);
ws_push(40);
ws_fetch();
ws_store();

    //label scan_number_start
ws_push(43);
ws_fetch();
ws_fetch();
    // got the char

ws_dup();
ws_jz(scan_number_positive_pop);

ws_dup();
ws_push(1);
ws_sub();
ws_jz(scan_number_negative_pop);

ws_dup();
ws_push(2);
ws_sub();
ws_jz(scan_number_no_sign_pop_next);//maybe error, number without sign

ws_jump(scan_number_finished_error);

ws_label(scan_number_finished_error);
ws_pushs("error: unknown type (0-2): ");
ws_call(prints);
ws_outn();
ws_push(0);
ws_call(printsln);
// debug_printheap
// debug_printstack
ws_jump(scan_number_finished);

ws_label(scan_number_positive_pop);
ws_drop();
ws_push(42);
ws_push(1);
ws_store();
ws_jump(scan_number_scan_digits);

ws_label(scan_number_negative_pop);
ws_drop();
ws_push(42);
ws_push(-1);
ws_store();
ws_jump(scan_number_scan_digits);

ws_label(scan_number_scan_digits);
ws_push(43);
ws_fetch();
ws_push(1);
ws_add();
ws_push(43);
ws_swap();
ws_store();
    // pointer to next char

ws_push(43);
ws_fetch();
ws_fetch();
    // got the char

ws_dup();
ws_jz(scan_number_digit_0_pop);

ws_dup();
ws_push(1);
ws_sub();
ws_jz(scan_number_digit_1_pop);

ws_dup();
ws_push(2);
ws_sub();
ws_jz(scan_number_finished_pop_next);

ws_jump(scan_number_finished_error);

ws_label(scan_number_digit_0_pop);
ws_drop();
ws_push(41);
ws_push(41);
ws_fetch();
ws_push(2);
ws_mul();
ws_push(0);
ws_add();
ws_store();
ws_jump(scan_number_scan_digits);

ws_label(scan_number_digit_1_pop);
ws_drop();
ws_push(41);
ws_push(41);
ws_fetch();
ws_push(2);
ws_mul();
ws_push(1);
ws_add();
ws_store();
ws_jump(scan_number_scan_digits);

ws_label(scan_number_no_sign_pop_next);
ws_pushs("warning: number without a sign");
ws_call(printsln);

ws_jump(scan_number_finished_pop_next);

ws_label(scan_number_finished_pop_next);
ws_drop();

ws_push(43);
ws_fetch();
ws_push(1);
ws_add();
ws_push(43);
ws_swap();
ws_store();
    // pointer to next char
ws_label(scan_number_finished);

ws_push(43);
ws_fetch();
ws_push(40);
ws_fetch();
ws_sub();
    // length

ws_push(41);
ws_fetch();
ws_push(42);
ws_fetch();
ws_mul();
    // number

ws_return();



// scan label
//   a = 0, b = 1, c = ende
//   next a,b are signiture (tranlated to number)
//   first c is terminator
// parameter
// [1] start of number
// return
// [1] number
// [2] length (including terminating 'c')
// variables
// [40] start pointer
// [41] number
// [43] actual pointer

ws_label(scan_label);
ws_push(41);
ws_push(1);
ws_store();

ws_push(40);
ws_swap();
ws_store();

ws_push(43);
ws_push(40);
ws_fetch();
ws_store();

ws_label(scan_label_scan_digits);
ws_push(43);
ws_fetch();
ws_fetch();
    // got the char

ws_push(43);
ws_fetch();
ws_push(1);
ws_add();
ws_push(43);
ws_swap();
ws_store();
    // pointer to next char

ws_dup();
ws_jz(scan_label_digit_0_pop);

ws_dup();
ws_push(1);
ws_sub();
ws_jz(scan_label_digit_1_pop);

ws_dup();
ws_push(2);
ws_sub();
ws_jz(scan_label_finished_pop);

ws_jump(scan_label_finished_error);

ws_label(scan_label_finished_error);
ws_pushs("error: unknown type (0-2): ");
ws_call(prints);
ws_outn();
ws_push(0);
ws_call(printsln);
// debug_printheap
// debug_printstack
ws_jump(scan_label_finished);

ws_label(scan_label_digit_0_pop);
ws_drop();
ws_push(41);
ws_push(41);
ws_fetch();
ws_push(2);
ws_mul();
ws_push(0);
ws_add();
ws_store();
ws_jump(scan_label_scan_digits);

ws_label(scan_label_digit_1_pop);
ws_drop();
ws_push(41);
ws_push(41);
ws_fetch();
ws_push(2);
ws_mul();
ws_push(1);
ws_add();
ws_store();
ws_jump(scan_label_scan_digits);

ws_label(scan_label_finished_pop);
ws_drop();

ws_label(scan_label_finished);

ws_push(43);
ws_fetch();
ws_push(40);
ws_fetch();
ws_sub();
    // length

ws_push(41);
ws_fetch();
    // signiture

ws_return();











// translates
// chars -> 0,1,2
// ' ' -> 0
// '\t' -> 1
// '\n' -> 2
//
// parameter:
// [1] start code
// [2] end code
// [3] start compiled code
// return
// [1] end compiled code
//
// variables
// [1] start code
// [2] end code
// [3] start compiled code
// [4] code pointer
// [5] compiled code pointer
// //stack:
// //instruction pointer
ws_label(compile_byte_code);

ws_push(3);
ws_swap();
ws_store();

ws_push(2);
ws_swap();
ws_store();

ws_push(1);
ws_swap();
ws_store();

//  push 10
//    push 1
//  retrive
//store

ws_push(4);
  // start of code
ws_push(1);
ws_fetch();
ws_store();

ws_push(5);
  // start of compiled code
ws_push(3);
ws_fetch();
ws_store();

// main loop

ws_jump(compile_byte_code_loop_restart);

ws_label(compile_byte_code_loop_restart_pop);
ws_drop();


ws_label(compile_byte_code_loop_restart);
ws_push(4);
ws_fetch();
ws_push(2);
ws_fetch();
ws_sub();

// finished, all data done
ws_jpz(compile_byte_code_finished);

ws_push(4);
ws_fetch();
ws_fetch();
ws_call(gettype);
ws_dup();

//increment code pointer
ws_push(4);
ws_push(4);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_jn(compile_byte_code_loop_restart_pop);

ws_push(5);
ws_fetch();
ws_swap();
ws_store();

//increment compiled code pointer
ws_push(5);
ws_push(5);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_jump(compile_byte_code_loop_restart);



ws_label(compile_byte_code_finished);

// compiled code pointer
ws_push(5);
ws_fetch();

// end of compiled code on stack
ws_return();








ws_label(gettype);
// parameter
// 1: char
// retrun
// 1: type ' ' is 0, '\t' is 1, '\n' is 2. else is -1

ws_dup(); ws_push(32); ws_sub();//space
//test 97  ;'a'
ws_jz(gettype_space);

ws_dup(); ws_push(9); ws_sub();//tab
//test 98  ;'b'
ws_jz(gettype_tab);

ws_dup(); ws_push(10); ws_sub();//return
//test 99    ;'c'
ws_jz(gettype_return);


ws_drop();
ws_push(-1);
ws_jump(gettype_ret);


ws_label(gettype_space);
ws_drop();
ws_push(0);
ws_jump(gettype_ret);

ws_label(gettype_tab);
ws_drop();
ws_push(1);
ws_jump(gettype_ret);

ws_label(gettype_return);
ws_drop();
ws_push(2);
ws_jump(gettype_ret);

ws_label(gettype_ret);
ws_return();






// program einlesen
// parameter:
//   1: where to store the data
// return:
//   1: where data ends
// var 1: status where in "\n\n\nquit\n\n\n" we are - 0 = at beginning
// var 2: place where to store data
// var 3: last char


ws_label(read_program);

// param 1 to [2]
ws_push(2);
ws_swap();
ws_store();

ws_label(read_start_reset);

ws_push(1);
ws_push(0);
ws_store();

ws_jump(read_start_noreset);

ws_label(read_start_noreset_pop);
ws_drop();

ws_label(read_start_noreset);


ws_push(2);
ws_fetch();
ws_readc();//number to heap
//Nummer einlesen

//debug
//retrive 1
//outn

//number to stack
ws_push(3);
ws_push(2);
ws_fetch();
ws_fetch();
ws_store();


ws_push(2);
ws_push(2);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_push(1); ws_fetch();
ws_dup(); ws_push(0); ws_sub();
ws_jz(onPos0_pop);

ws_dup(); ws_push(1); ws_sub();
ws_jz(onPos1_pop);

ws_dup(); ws_push(2); ws_sub();
ws_jz(onPos2_pop);

ws_dup(); ws_push(3); ws_sub();
ws_jz(onPos3_pop);

ws_dup(); ws_push(4); ws_sub();
ws_jz(onPos4_pop);

ws_dup(); ws_push(5); ws_sub();
ws_jz(onPos5_pop);

ws_dup(); ws_push(6); ws_sub();
ws_jz(onPos6_pop);

ws_dup(); ws_push(7); ws_sub();
ws_jz(onPos7_pop);

ws_dup(); ws_push(8); ws_sub();
ws_jz(onPos8_pop);

ws_jump(onPos9_pop);




ws_label(onPos0_pop);
ws_label(onPos1_pop);
ws_label(onPos2_pop);
ws_label(onPos7_pop);
ws_label(onPos8_pop);
ws_label(onPos9_pop);
ws_drop();

ws_push(3); ws_fetch();

ws_dup(); ws_push(13); ws_sub();
ws_jz(read_start_noreset_pop);

ws_dup(); ws_push(10); ws_sub();
ws_jz(read_onNext_pop);

ws_drop();

ws_jump(read_start_reset);



ws_label(onPos3_pop);
ws_drop();

ws_push(3); ws_fetch();
ws_dup(); ws_push(10); ws_sub();// enter
ws_jz(read_start_noreset_pop);

ws_dup(); ws_push(13); ws_sub();// enter
ws_jz(read_start_noreset_pop);

ws_dup(); ws_push(113); ws_sub();// 'q'
ws_jz(read_onNext_pop);
ws_drop();
ws_jump(read_start_reset);


ws_label(onPos4_pop);
ws_drop();

ws_push(3); ws_fetch();
ws_dup(); ws_push(117); ws_sub();// 'u'
ws_jz(read_onNext_pop);
ws_drop();
ws_jump(read_start_reset);


ws_label(onPos5_pop);
ws_drop();

ws_push(3); ws_fetch();
ws_dup(); ws_push(105); ws_sub();// 'i'
ws_jz(read_onNext_pop);
ws_drop();
ws_jump(read_start_reset);


ws_label(onPos6_pop);
ws_drop();

ws_push(3); ws_fetch();
ws_dup(); ws_push(116); ws_sub();// 't'
ws_jz(read_onNext_pop);
ws_drop();
ws_jump(read_start_reset);


ws_label(read_onNext_pop);
ws_push(1); ws_fetch();
ws_push(1); ws_add();
ws_push(1); ws_swap(); ws_store();

ws_push(1); ws_fetch();
ws_push(10); ws_sub();
ws_jz(read_end);

ws_jump(read_start_noreset);


ws_label(read_end);
ws_push(2);
ws_fetch();

ws_return();







//equal to 10
ws_push(3);
ws_fetch();
ws_push(10);
ws_sub();
ws_jz(onReturn);
//pop

//equal to 13

ws_jump(read_start_reset);




ws_label(onReturn);
ws_push(1);

ws_push(1);
ws_fetch();
ws_push(1);
ws_add();
ws_store();


ws_push(1);
ws_fetch();

//  doub
//  outN

ws_push(6);
ws_sub();

ws_jn(read_start_noreset);

// print_atom_list();
}
