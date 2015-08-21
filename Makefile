
CFLAGS=-O3 -Wall -Wextra
EXE=wsc ws2c wsa blockquote

all: $(EXE)

ws2c: ws2c.o

ws2c.o: ws2c.c ws_engine.h ws_engine_txt.h

ws_engine_txt.h: blockquote ws_engine.h
	( echo 'char * header =' ;\
	./blockquote -f ws_engine.h ;\
	echo ';' ) > ws_engine_txt.h

clean:
	-rm -f $(EXE)
	-rm -f ws_engine_txt.h
	-rm -f ws2c.o

