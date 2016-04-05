all:
	gcc -g -c -fPIC -std=c99 helpers.c -o helpers.o
	gcc -g -c -fPIC -std=c99 -I. -L. bufio.c -o bufio.o -lhelpers
	gcc -g -shared helpers.o -o libhelpers.so
	gcc -g -shared bufio.o -o libbufio.so

clean:
	rm -f helpers.o
	rm -f libhelpers.so
	rm -f bufio.o
	rm -f libbufio.so
