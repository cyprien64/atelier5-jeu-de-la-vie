CC = gcc
CFLAGS = -Wall -Wextra -std=c99

conway_battle: main.c
	$(CC) $(CFLAGS) main.c -o conway_battle

clean:
	rm -f conway_battle
