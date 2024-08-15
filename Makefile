CC = gcc
CFLAGS = -Wall -W -pedantic -std=c99

all: fetchext2

fetchext2: main.c
	$(CC) $(CFLAGS) -o fetchext2 main.c

clean:
	rm -f fetchext2
