CC=gcc
CFLAGS=-g -Wall
DBGCFLAGS=-DDEBUG -g -Wall
EXECUTABLES=crp

crp: crp.c
	${CC} ${CFLAGS} -o $@ $^
crp_d: crp.c
	${CC} ${DBGCFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
