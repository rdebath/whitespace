
CFLAGS=-O3 -Wall -Wextra
EXE=wsc ws2c wsa

all: $(EXE)

clean:
	-rm -f $(EXE)
