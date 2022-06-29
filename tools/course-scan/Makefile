all: scanner

%.o: %.c
	gcc -c -o $@ $<

scanner: scanner.o memory.o
	gcc -o $@ $^ -lm

.phony: clean

clean:
	rm scanner *.o
