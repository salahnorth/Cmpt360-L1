# Student's Name: Salah Mohamed & Vy Tran

CC := gcc
CFLAGS := -std=c11 -Wall
LFLAGS := -lc -lm
Object = procSim.o dStruct.o dStruct.h 
target = lab6

ALGOR_FIFO := 1
ALGOR_SJF := 2
ALGOR_RR := 3
ALGOR_MLFQ := 4

$(target):$(Object)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
.PHONY: clean, conform, run
clean:
	rm -f *.o $(target)
conform:
	./indent -linux *.c -o *.c
run-fifo:
	./$(target) $(ALGOR_FIFO)

run-sjf:
	./$(target) $(ALGOR_SJF)
	
run-rr:
	./$(target) $(ALGOR_RR)
	
run-mlfq:
	./$(target) $(ALGOR_MLFQ)
