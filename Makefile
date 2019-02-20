#compiler to be used gcc/g++
CC=gcc
#compiling flags
CFLAGS=-g -c -Wall -ansi -std=c90 -pedantic
#linking flags
LFLAGS=-g -Wall -ansi -std=c90
#lists all c source file names
SRC= $(wildcard *.c)
#create object file list from src list
OBJ= $(SRC:.c=.o)
#desired executable file name
TARGET= assembler
assembler: $(OBJ)
	$(CC) $(LFLAGS) $^ -o $@
assembler.o: assembler.c parser.h
	$(CC) $(CFLAGS) $< -o $@
parser.o: parser.c
	$(CC) $(CFLAGS) $< -o $@
#delete executables and object files
clean:
	@rm -f *.o *.exe $(TARGET)
	@echo Directory Cleaned
#NOTE TO SELF: for bash/powershell, use "make clean" -> "make" when switching OS to avoid "cannot execute binary file" error.
#run throught all of the input files.
loop:
	@echo ***running:
	@for i in `seq 1 1` ; do \
		echo "<input$$i.txt>output$$i.txt" ; \
		./$(TARGET)<input$$i.txt>output$$i.txt ; \
	done
run:
	./assembler File1.AS