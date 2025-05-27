## Compiler and Flags
CC = gcc
CFLAGS = -Wall -g -std=gnu99 -Werror -lm

## List of Object files
OBJS = cpu_info.o memory_info.o main.o

## Executable
all: myProgram

## Linking Steps
myProgram: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

## Compile Each object
main.o: main.c cpu_info.h memory_info.h
	$(CC) $(CFLAGS) -c main.c

cpu_info.o: cpu_info.c cpu_info.h
	$(CC) $(CFLAGS) -c cpu_info.c

memory_info.o: memory_info.c memory_info.h
	$(CC) $(CFLAGS) -c memory_info.c

## Clean and Help rule
.PHONY: clean
clean:
	rm -f *.o myProgram

.PHONY: help
help:
	@sed -n 's/^## //p' $<