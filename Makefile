OBJ = rrtTester.o display.o rrtMaker.o

all:	display.o rrtMaker.o rrtTester.o
	gcc -Wall -o rrtTester $(OBJ)  -lm -lX11 -g 

display:	display.c display.h obstacles.h
	gcc -c display.c 

rrtMaker:	rrtMaker.c display.h obstacles.h
	gcc -c rrtMaker.c

clean:
	rm -f $(OBJ)

cleanall:
	rm -f rrtTester $(OBJ)
