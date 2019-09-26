CC = gcc
CFLAGS = -g -std=gnu99
THRD = -lpthread


all:
	 $(CC) $(CFLAGS) mail.c -o mail $(THRD)

clean:
	rm mail *.o
