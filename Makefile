CC="icc"
CFLAGS=-Wall

finder:
	$(CC) finder.c ./libs/gmp-6.0.0/bin/libgmp.a ./libs/gsl-1.16/bin/libgsl.a -lgslcblas -fopenmp -I ./libs/gsl-1.16/ -I ./libs/gmp-6.0.0/ -o finder
