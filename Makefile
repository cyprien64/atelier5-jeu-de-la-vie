CC = gcc
CFLAGS = -Wall -Wextra -std=c99

life: main.c
	$(CC) $(CFLAGS) main.c -o life

clean:
	rm -f life
