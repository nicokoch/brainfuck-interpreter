IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -O3 -Wall

ODIR=.

_DEPS = brainfuck.h optimize.h shell.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o brainfuck.o optimize.o shell.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

brainfuck-interpreter: $(OBJ)
	gcc -o ../$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ ../brainfuck-interpreter
