CC=g++
CFLAGS=

all:OneD.out

OneD.out:OneD.cpp
	$(CC) $(CFLAGS) -o $@ $^

run:
	./OneD.out

clean:
	rm *.out
