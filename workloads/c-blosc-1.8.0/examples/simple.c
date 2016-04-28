/*
    Copyright (C) 2014  Francesc Alted
    http://blosc.org
    License: MIT (see LICENSE.txt)

    Example program demonstrating use of the Blosc filter from C code.

    To compile this program:

    gcc simple.c -o simple -lblosc -lpthread

    or, if you don't have the blosc library installed:

    gcc -O3 -msse2 simple.c ../blosc/*.c -I../blosc -o simple -lpthread

    Using MSVC on Windows:

    cl /arch:SSE2 /Ox /Fesimple.exe /Iblosc examples\simple.c blosc\blosc.c blosc\blosclz.c blosc\shuffle.c blosc\shuffle-sse2.c blosc\shuffle-generic.c blosc\bitshuffle-generic.c blosc\bitshuffle-sse2.c

    To run:

    $ ./simple
    Blosc version info: 1.4.2.dev ($Date:: 2014-07-08 #$)
    Compression: 4000000 -> 158494 (25.2x)
    Decompression succesful!
    Succesful roundtrip!

*/

#include <stdio.h>
#include <math.h>
#include <blosc.h>
#include "measure_time.h"
#include "fiber.h"
#include "workloads.h"

#define SIZE 500*500*200
#define SHAPE {100,100,100}
#define CHUNKSHAPE {1,100,100}

typedef struct { int h, w; double *x;} matrix_t, *matrix;

matrix mat_new(int h, int w)
{
  matrix r = malloc(sizeof(matrix_t) + sizeof(double) * w * h);
  r->h = h, r->w = w;
  r->x = (double*)(r + 1);
  return r;
}

inline double dot(double *a, double *b, int len, int step)
{
  double r = 0;
  while (len--) {
    r += *a++ * *b;
    b += step;
  }
  return r;
}

matrix mat_mul(matrix a, matrix b)
{
  matrix r;
  double *p, *pa;
  int i, j;
  if (a->w != b->h) return 0;

  r = mat_new(a->h, b->w);
  p = r->x;
  for (pa = a->x, i = 0; i < a->h; i++, pa += a->w)
    for (j = 0; j < b->w; j++)
      *p++ = dot(pa, b->x + j, a->w, b->w);
  return r;
}

int cblosc_simple_start(void *args){
  static float data[SIZE];
  static float data_out[SIZE];
  static float data_dest[SIZE];
  int isize = SIZE*sizeof(float), osize = SIZE*sizeof(float);
  int dsize = SIZE*sizeof(float), csize;
  int i;

  /*double data_a[SIZE/2], data_b[SIZE/2];

  for(i=0; i<SIZE; i++){
    data[i] = i;
  }

  for (i=0; i<SIZE/2; i++) {
    data_a[i] = data[i];
    data_b[i] = data[i + SIZE/2];
  }

  int tmp = sqrt(SIZE/2);
  matrix_t a = {tmp, tmp, data_a};
  matrix_t b = {tmp, tmp, data_b};

  uint64_t begin = RDTSCP();
  matrix c = mat_mul(&a, &b);
  uint64_t end = RDTSCP();
  double time = (1.0*(end-begin))/FREQ;

  printf("Matrix time: %f\n",time);

  for (i=0;i<SIZE/2;i++)
  {
    data[i]= c->x[i];
  }
*/

  /* Register the filter with the library */
  printf("Blosc version info: %s (%s)\n",
	 BLOSC_VERSION_STRING, BLOSC_VERSION_DATE);

  /* Initialize the Blosc compressor */
  blosc_init();

  /* Compress with clevel=5 and shuffle active  */
  //fiber_change(2, );
  uint64_t begin = RDTSCP();
  csize = blosc_compress(5, 1, sizeof(float), isize, data, data_out, osize);
  uint64_t end = RDTSCP();
  double time = (1.0*(end-begin))/FREQ;
  printf("time to compress :%f\n",time);
  if (csize == 0) {
    printf("Buffer is uncompressible.  Giving up.\n");
    return 1;
  }
  else if (csize < 0) {
    printf("Compression error.  Error code: %d\n", csize);
    return csize;
  }

  printf("Compression: %d -> %d (%.1fx)\n", isize, csize, (1.*isize) / csize);

  /* Decompress  */
  dsize = blosc_decompress(data_out, data_dest, dsize);
  if (dsize < 0) {
    printf("Decompression error.  Error code: %d\n", dsize);
    return dsize;
  }

  printf("Decompression succesful!\n");

  /* After using it, destroy the Blosc environment */
  blosc_destroy();

  for(i=0;i<SIZE;i++){
    if(data[i] != data_dest[i]) {
      printf("Decompressed data differs from original!\n");
      return -1;
    }
  }

  printf("Succesful roundtrip!\n");
  return 0;
}
