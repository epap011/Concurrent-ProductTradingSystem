#Author: Efthymis Papageorgiou 4340

#Compiler
CC = gcc
#compiler flags
CFLAGS = -Wall

exec: main.o concurrent_dll.o concurrent_ht.o
	$(CC) $(CFLAGS) main.o concurrent_dll.o concurrent_ht.o -o a.out

main.o:	main.c
	$(CC) $(CFLAGS) -c main.c

concurrent_dll.o: concurrent_dll.c
	$(CC) $(CFLAGS) -c concurrent_dll.c

concurrent_ht.o: concurrent_ht.c
	$(CC) $(CFLAGS) -c concurrent_ht.c

clean:
	rm -rf *o a.out