TARGET = Load Change Query Print Clean
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall
DEPS = header.o 

.PHONY: default all clean

default: $(TARGET)
all: default

HEADERS = $(wildcard *.h)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

Load: load.o $(DEPS)
	$(CC) load.c $(DEPS) -Wall $(LIBS) -o $@

Change: change.o $(DEPS)
	$(CC) change.c $(DEPS) -Wall $(LIBS) -o $@

Query: query.o $(DEPS)
	$(CC) query.c $(DEPS) -Wall $(LIBS) -o $@

Print: print.o $(DEPS)
	$(CC) print.c $(DEPS) -Wall $(LIBS) -o $@
	
Clean: clean.o $(DEPS)
	$(CC) clean.c $(DEPS) -Wall $(LIBS) -o $@




clean:
	-rm -f *.o
	-rm -f $(TARGET)
