COMPILER = gcc
CCFLAGS = -pedantic -Wall -Werror -std=c99 -g
AR = ar

all: index

index: indexer.o libtrie.a
	$(COMPILER) $(CCFLAGS) indexer.o libtrie.a -o index

libtrie.a: itrie.o
	$(AR) -r libtrie.a itrie.o

itrie.o: itrie.c
	$(COMPILER) $(CCFLAGS) -c itrie.c

indexer.o: indexer.c
	$(COMPILER) $(CCFLAGS) -c indexer.c

clean:
	rm -f *.o *.a *.gch index
