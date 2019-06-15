
CFLAGS = -std=c11 -Wall -Werror -march=native -O2 -D_GNU_SOURCE
LDFLAGS = -lpthread
PROGNAME = setpid

default: all

all:
	gcc $(CFLAGS) $(LDFLAGS) setpid.c -o $(PROGNAME)

clean:
	@rm -v $(PROGNAME) 2>/dev/null || true

