
//memcopy
// copys memory from one place to another the places may overlap
// parameter
// 3: source
// 2: dest
// 1: length
// local variables
// 1: source pointer
// 2: dest pointer
// 3: left count
ws_label(memcopy);
ws_label(mem_copy);
ws_label(mem_move);

ws_push(3);
ws_swap();
ws_store();
// left count

ws_push(2);
ws_swap();
ws_store();
// dest pointer

ws_push(1);
ws_swap();
ws_store();
// source pointer


ws_push(3); ws_fetch();
ws_jzn(memcopy_end);//no more left

ws_push(1); ws_fetch();
ws_push(2); ws_fetch();
ws_sub();

ws_jz(memcopy_end);//source = dest

ws_push(1); ws_fetch();
ws_push(2); ws_fetch();
ws_sub();

ws_jn(memcopy_loop_dest_greater_source_begin);
ws_jump(memcopy_loop_source_greater_dest);


ws_label(memcopy_loop_source_greater_dest);
ws_push(2);
ws_fetch();
ws_push(1);
ws_fetch();
ws_fetch();
ws_store();

ws_push(2);
ws_push(2);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_push(1);
ws_push(1);
ws_fetch();
ws_push(1);
ws_add();
ws_store();

ws_push(3);
ws_push(3);
ws_fetch();
ws_push(1);
ws_sub();
ws_store();

ws_push(3); ws_fetch();

ws_jz(memcopy_end);
ws_jump(memcopy_loop_source_greater_dest);



ws_label(memcopy_loop_dest_greater_source_begin);
// dest > source so we need to go backwards throug the memory
ws_push(2);
ws_push(2);
ws_fetch();
ws_push(3);
ws_fetch();
ws_add();
ws_push(1);
ws_sub();
ws_store();

ws_push(1);
ws_push(1);
ws_fetch();
ws_push(3);
ws_fetch();
ws_add();
ws_push(1);
ws_sub();
ws_store();

ws_label(memcopy_loop_dest_greater_source);
ws_push(2);
ws_fetch();
ws_push(1);
ws_fetch();
ws_fetch();
ws_store();

ws_push(2);
ws_push(2);
ws_fetch();
ws_push(1);
ws_sub();
ws_store();

ws_push(1);
ws_push(1);
ws_fetch();
ws_push(1);
ws_sub();
ws_store();

ws_push(3);
ws_push(3);
ws_fetch();
ws_push(1);
ws_sub();
ws_store();

ws_push(3); ws_fetch();

ws_jz(memcopy_end);
ws_jump(memcopy_loop_dest_greater_source);


ws_label(memcopy_end);

ws_return();







// parameter
// [1] from
// [2] count

ws_label(mem_zero);

ws_label(mem_zero_start);
//from->count
ws_dup();
ws_jz(mem_zero_end);

ws_swap();//count->from

ws_dup();
ws_push(0);
ws_store();

ws_push(1);
ws_add();

ws_swap();//from->count

ws_push(1);
ws_sub();
ws_jump(mem_zero_start);

ws_label(mem_zero_end);
ws_drop();
ws_drop();
ws_return();















// nummeriere speicher von [1] - [2] // for debuging
ws_label(numeriere);
ws_push(2);
ws_swap();
ws_store();

ws_push(1);
ws_swap();
ws_store();

ws_push(1);
ws_fetch();

ws_label(numeriere_start);

ws_dup();
ws_dup();
ws_store();

ws_push(1);
ws_add();

ws_dup();

ws_push(2);
ws_fetch();
ws_sub();

ws_jz(numeriere_end);

ws_jump(numeriere_start);

ws_label(numeriere_end);
ws_drop();
ws_return();



