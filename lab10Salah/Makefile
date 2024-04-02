# Student's Name: Salah Mohamed & Vy Tran

CC := gcc
CFLAGS := -Wall
LFLAGS := -lc -lm
Object = testfindme.o findme.o findme.h 
target = lab10
testObject = testglob.o findme.o findme.h

$(target):$(Object)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
	
globtest: $(testObject)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
.PHONY: clean, conform, run
clean:
	rm -f *.o $(target) globtest
conform:
	./indent -linux *.c -o *.c
