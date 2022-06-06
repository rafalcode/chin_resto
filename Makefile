CC=gcc
CFLAGS=-g -Wall
DBGCFLAGS=-DDEBUG -g -Wall
EXECUTABLES=crp0 crp

crp0: crp0.c
	${CC} ${CFLAGS} -o $@ $^
crp0_d: crp0.c
	${CC} ${DBGCFLAGS} -o $@ $^

crp: crp.c
	${CC} ${CFLAGS} -o $@ $^
crp_d: crp.c
	${CC} ${DBGCFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
