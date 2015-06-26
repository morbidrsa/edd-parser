CFLAGS=-Wall -Wextra -O2 -g

all: edd-parser.c
	$(CC) $(CFLAGS) -o edd-parser edd-parser.c

clean:
	rm -f edd-parser
