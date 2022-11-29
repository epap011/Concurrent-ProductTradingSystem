####################################
# @author Papageorgiou Efthymios   #
# @version v.1                     #
####################################

#Compiler
CC = gcc
#compiler flags
CFLAGS = -Wall

build: main.o concurrent_dll.o concurrent_ht.o concurrent_stack.o
	$(CC) $(CFLAGS) main.o concurrent_dll.o concurrent_ht.o concurrent_stack.o -o a.out

main.o:	main.c
	$(CC) $(CFLAGS) -c main.c

concurrent_dll.o: concurrent_dll.c
	$(CC) $(CFLAGS) -c concurrent_dll.c

concurrent_ht.o: concurrent_ht.c
	$(CC) $(CFLAGS) -c concurrent_ht.c

concurrent_stack.o:	concurrent_stack.c
	$(CC) $(CFLAGS) -c concurrent_stack.c

clean:
	rm -rf *o a.out