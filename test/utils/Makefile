# "test/utils/Makefile"

MAKE = make
SHELL = /bin/sh

ROOT = ../..

CC = gcc
LD = $(CC)
CFLAGS = -O3 -Wall -c -g -I$(ROOT)
LDFLAGS = -lc -lstdc++

TARGETS = hashlist_test

.SUFFIXES: .cc .o

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

default: all

all: $(TARGETS)

TEST_OBJ = hashlist_test.o
hashlist_test: $(TEST_OBJ)
	$(LD) $(LDFLAGS) $(TEST_OBJ) -o $@

clean:
	-rm -f *.o
	-rm -f *.tmp.*
	-rm -f *.core

clobber: clean
	-rm -f $(TARGETS)

# header dependencies
hashlist_test.o: $(ROOT)/hashlist.h $(ROOT)/hashlist_template_methods.h
hashlist_test.o: $(ROOT)/list_of_ptr.h $(ROOT)/list_of_ptr_template_methods.h

