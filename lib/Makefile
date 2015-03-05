all:
	gcc -c -fPIC -std=c99 helpers.c -o helpers.o
	gcc -shared helpers.o -o libhelpers.so

clean:
	rm -f helpers.o
	rm -f libhelpers.so
