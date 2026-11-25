CC = gcc
CFLAGS = -Wall -Wextra -std=c99

avions: main.c
	$(CC) $(CFLAGS) main.c -o avions

clean:
	rm -f avions
