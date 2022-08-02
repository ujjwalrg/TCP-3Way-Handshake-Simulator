client: client.o
	gcc -o client client.o
#below code means to compile all the c files to o files
#%.o : %.c
#	gcc -ansi -pedantic-errors -Wall -c $<
client.o: client.c
	gcc -c client.c

clean:
	rm *.o client


