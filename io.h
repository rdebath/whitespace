
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

