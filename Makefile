CC=gcc
CFLAGS=-Wall

finder:
	$(CC) -o finder finder.c -l gsl -l gmp -l gslcblas
