#!/bin/bash

g++ -c pbzip2.cpp -m64 -DARCH_x86_64 -fPIC -I../../libfiber-master/include/
g++ -c ErrorContext.cpp -m64 -DARCH_x86_64 -fPIC -I../../libfiber-master/include/
g++ -c BZ2StreamScanner.cpp -m64 -DARCH_x86_64 -fPIC -I../../libfiber-master/include/

g++ -o test BZ2StreamScanner.o ErrorContext.o pbzip2.o