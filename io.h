
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

