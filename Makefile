#compiler to be used gcc/g++
CC=gcc
#compiling flags
CFLAGS=-g -c -Wall -ansi -std=c90 -pedantic
#linking flags
LFLAGS=-g -Wall -ansi -std=c90 -pedantic
#lists all c source file names
SRC= $(wildcard *.c)
#create object file list from src list
OBJ= $(SRC:.c=.o)
#desired executable file name
TARGET= assembler
assembler: $(OBJ)
	$(CC) $(LFLAGS) $^ -o $@
assembler.o: assembler.c database.h utility.h scanner.h data_structure.h
	$(CC) $(CFLAGS) $< -o $@
data_structure.o: data_structure.c database.h utility.h
	$(CC) $(CFLAGS) $< -o $@
error.o: error.c database.h utility.h
	$(CC) $(CFLAGS) $< -o $@
parser.o: parser.c utility.h database.h error.h 
	$(CC) $(CFLAGS) $< -o $@
second_scan.o: second_scan.c database.h data_structure.h error.h utility.h  
	$(CC) $(CFLAGS) $< -o $@
first_scan.o: first_scan.c database.h data_structure.h error.h parser.h utility.h
	$(CC) $(CFLAGS) $< -o $@
utility.o: utility.c
	$(CC) $(CFLAGS) $< -o $@
#delete executables and object files
clean:
	@rm -f *.o *.exe *.ent *.ob *.ext *.txt $(TARGET)
	@echo Directory Cleaned
file:
	@echo ***running:
	@for i in `seq 1 6` ; do \
		echo ">file$$i-redirection.txt" ; \
		./$(TARGET) file$$i>file$$i-redirection.txt ; \
	done
error:
	@echo ***running:
	@for i in `seq 1 4` ; do \
		echo ">err$$i-redirection.txt" ; \
		./$(TARGET) errors$$i>err$$i-redirection.txt ; \
	done
run:
	./assembler File1.AS