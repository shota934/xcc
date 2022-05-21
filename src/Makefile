CC=gcc
OPT=-c
CFLAGS=-g

OBJS= main.o parser.o list.o lex.o mem.o dump.o file.o gen.o env.o util.o error.o cpp.o optinfo.o symbol.o compound_type.o stack.o com.o srcinfo.o set.o map.o macro.o func.o tych.o enumdef.o sema.o value.o ope.o obj.o
TARGET=xcc

xcc: ${OBJS}
	${CC} ${OBJS} ${CFLAGS} -o ${TARGET}

main.o : main.c
	${CC} ${OPT} ${CFLAGS} main.c

parser.o : parser.c
	${CC} ${OPT} ${CFLAGS} parser.c

lex.o : lex.c
	${CC} ${OPT} ${CFLAGS} lex.c

list.o : list.c
	${CC} ${OPT} ${CFLAGS} list.c

mem.o:  mem.c
	${CC} ${OPT} ${CFLAGS} mem.c

dump.o: dump.c
	${CC} ${OPT} ${CFLAGS} dump.c

file.o: file.c
	${CC} ${OPT} ${CFLAGS} file.c

gen.o: gen.c
	${CC} ${OPT} ${CFLAGS} gen.c

env.o: env.c
	${CC} ${OPT} ${CFLAGS} env.c

util.o : util.c
	${CC} ${OPT} ${CFLAGS} util.c

error.o : error.c
	${CC} ${OPT} ${CFLAGS} error.c

cpp.o   : cpp.c
	${CC} ${OPT} ${CFLAGS} cpp.c

optinfo.o : optinfo.c
	${CC} ${OPT} ${CFLAGS} optinfo.c

typeutil.o : typeutil.c
	${CC} ${OPT} ${CFLAGS} typeutil.c

symbol.o : symbol.c
	${CC} ${OPT} ${CFLAGS} symbol.c

compound_type.o : compound_type.c
	${CC} ${OPT} ${CFLAGS} compound_type.c

stack.o : stack.c
	${CC} ${OPT} ${CFLAGS} stack.c

com.o : com.c
	${CC} ${OPT} ${CFLAGS} com.c

srcinfo.o : srcinfo.c
	${CC} ${OPT} ${CFLAGS} srcinfo.c

set.o : set.c
	${CC} ${OPT} ${CFLAGS} set.c

map.o : map.c
	${CC} ${OPT} ${CFLAGS} map.c

macro.o : macro.c
	${CC} ${OPT} ${CFLAGS} macro.c

func.o : func.c
	${CC} ${OPT} ${CFLAGS} func.c

tych.o : tych.c
	${CC} ${OPT} ${CFLAGS} tych.c

enumdef.o : enumdef.c
	${CC} ${OPT} ${CFLAGS} enumdef.c

sema.o : sema.c
	${CC} ${OPT} ${CFLAGS} sema.c

value.o : value.c
	${CC} ${OPT} ${CFLAGS} value.c

obj.o : obj.c
	${CC} ${OPT} ${CFLAGS} obj.c

ope.o : ope.c
	${CC} ${OPT} ${CFLAGS} ope.c

test: xcc
	./test.sh

clean:
	rm *.o
	rm ${TARGET}
	rm tmp*
	rm *.s


