#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE * ofd;

void do_file(FILE* ifd);
int strnmode = 0, has_str = 0;
int force_on = 0;

int
main(int argc, char **argv) {
    char ** filelist = 0;
    int filecount = 0;
    int ar, opton = 1;

    ofd = stdout;

    filelist = calloc(argc, sizeof*filelist);
    for(ar=1; ar<argc; ar++) {
	if (opton && argv[ar][0] == '-' && argv[ar][1] != 0) {
	    if (argv[ar][1] == '-' && argv[ar][2] == 0) {
		opton = 0;
		continue;
	    }
	    if (!strcmp(argv[ar], "-f")) {
		force_on = strnmode = 1;
		continue;
	    }
	    fprintf(stderr, "Unknown option '%s'\n", argv[ar]);
	    exit(1);
	} else
	    filelist[filecount++] = argv[ar];
    }
    for(ar=0; ar<filecount; ar++) {
	if (strcmp(filelist[ar], "-") == 0)
	    do_file(stdin);
	else {
	    FILE * ifd;
	    if (!(ifd = fopen(filelist[ar], "r"))) {
		perror(filelist[ar]);
		exit(1);
	    } else {
		if (!force_on)
		    fprintf(ofd, "#line 1 \"%s\"\n", filelist[ar]);
		do_file(ifd);
		fclose(ifd);
	    }
	}
    }
    return 0;
}

void
write_buf(char * buf)
{
    int nl = 0, iq = 0;
    if(strnmode == 0) { fputs(buf, ofd); return; }

    if (*buf) {
	for(;*buf;buf++) {
	    if (*buf == '\n') { nl++; continue; }
	    if (!iq) {
		putc('\t', ofd);
		putc('"', ofd);
		has_str = iq = 1;
	    }
	    if (*buf == '"') { putc('\\', ofd); putc('"', ofd); continue; }
	    if (*buf == '\t') { putc('\\', ofd); putc('t', ofd); continue; }
	    if (*buf == '\f') { putc('\\', ofd); putc('f', ofd); continue; }
	    if (*buf == '\\') { putc('\\', ofd); putc('\\', ofd); continue; }
	    putc(*buf, ofd);
	}
	if (iq)
	    putc('"', ofd);
    }
    while(nl-->0)
	putc('\n', ofd);
}

void
do_file(FILE* ifd)
{
    char buf[BUFSIZ];
    int contflag = 0;

    while ((fgets(buf, sizeof(buf), ifd)) != 0) {
	int oc = contflag;
	contflag = (!*buf || buf[strlen(buf)-1] != '\n');
	if (oc) { write_buf(buf); continue; }

	if (strnmode && !force_on && !strncmp(buf, "#endif", 6) && strstr(buf, "QUOTE")) {
	    if (!has_str)
		fputs("\t\"\"\n", ofd);
	    else
		fputs("\n", ofd);
	    strnmode = 0;
	    continue;
	}
	if (strnmode > 1) { fputs("\"\\n\"", ofd); write_buf(buf); continue; }
	if (strnmode) { strnmode=2; write_buf(buf); continue; }

	if (!strncmp(buf, "#if", 3)
	    && (buf[3] != ' ' || buf[3] != '\t')
	    && strstr(buf, "QUOTE")) {

	    fputs("\n", ofd);
	    strnmode = 1; has_str = 0;
	    continue;
	}

	fputs(buf, ofd);
    }
}
