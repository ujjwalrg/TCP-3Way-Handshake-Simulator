server: server.o
	gcc -o server server.o
#below code means to compile all the c files to o files
#%.o : %.c
#	gcc -ansi -pedantic-errors -Wall -c $<
server.o: server.c
	gcc -c server.c

clean:
	rm *.o server


