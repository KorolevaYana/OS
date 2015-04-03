all:
	gcc -c -fPIC -std=c99 helpers.c -o helpers.o
	gcc -c -fPIC -std=c99 -I. -L. bufio.c -o bufio.o -lhelpers
	gcc -shared helpers.o -o libhelpers.so
	gcc -shared bufio.o -o libbufio.so

clean:
	rm -f helpers.o
	rm -f libhelpers.so
	rm -f bufio.o
	rm -f libbufio.so
