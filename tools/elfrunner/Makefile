CFLAGS=-m32 -Wall
LDFLAGS=-m32

all: elfrunner

elfrunner: elfrunner.o elf.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f elfrunner *.o
