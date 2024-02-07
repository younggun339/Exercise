.PHONY: test

CFLAGS=-I ../src -Wall -g -DSENTINEL

SRCS_DIR = ../src/

SRCS_OBJ =	rbtree.o\
			util.o\
			insert.o\
			erase.o\

test: test-rbtree
	./test-rbtree
	valgrind ./test-rbtree

test-rbtree: test-rbtree.o $(addprefix $(SRCS_DIR), $(SRCS_OBJ))

../src/rbtree.o:
	$(MAKE) -C ../src $(SRCS_OBJ)

clean:
	rm -f test-rbtree *.o
