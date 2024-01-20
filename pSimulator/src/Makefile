# Student's Name: Salah Mohamed & Vy Tran

CC := gcc
CFLAGS := -std=c11 -Wall
LFLAGS := -lc -lm
Object = procSim.o dStruct.o dStruct.h 
target = lab2

$(target):$(Object)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
.PHONY: clean, conform, run
clean:
	rm -f *.o $(target)
conform:
	./indent -linux *.c -o *.c
run:
	./$(target)

