# Makefile for dedup kernel

PREFIX=${PARSECDIR}/pkgs/kernels/dedup/inst/${PARSECPLAT}

TARGET=dedup.so
#change
CFLAGS += -Wall -fno-strict-aliasing -D_XOPEN_SOURCE=600 -Werror -m64 -DARCH_x86_64 -pthread -Wall -I../../../libfiber-master/include/ -I../../../libfiber-master/submodules/libev -D_REENTRANT -ggdb -fsplit-stack -DFIBER_STACK_SPLIT -DLINUX -DUSE_COMPILER_THREAD_LOCAL -DFIBER_FAST_SWITCHING -Isrc -Itest #-fno-aggressive-loop-optimizations

ostype=$(findstring solaris, ${PARSECPLAT})

ifeq "$(ostype)" "solaris"
    CFLAGS += -std=gnu99
endif

LIBS += -lm

DEDUP_OBJ = hashtable.o util.o dedup.o rabin.o encoder.o decoder.o mbuffer.o sha.o

# Uncomment the following to enable gzip compression
CFLAGS += -DENABLE_GZIP_COMPRESSION -fPIC
LIBS += -lz -L. -L../../../../bin ../../../libfiber-master/libfiber.so -lpthread -ldl -lm -lrt #change
LDFLAGS += -Werror -lm -m64 -DARCH_x86_64 -pthread -Wall -I../../../libfiber-master/include/ -I../../../libfiber-master/submodules/libev -D_REENTRANT -ggdb -fsplit-stack -DFIBER_STACK_SPLIT -DLINUX -DUSE_COMPILER_THREAD_LOCAL -DFIBER_FAST_SWITCHING #-fno-aggressive-loop-optimizations
# Uncomment the following to enable bzip2 compression
#CFLAGS += -DENABLE_BZIP2_COMPRESSION
#LIBS += -lbz2

CFLAGS += -DENABLE_PTHREADS -pthread
DEDUP_OBJ += queue.o binheap.o tree.o
#ifdef version
#  ifeq "$(version)" "pthreads"
#    CFLAGS += -DENABLE_PTHREADS -pthread
#    DEDUP_OBJ += queue.o binheap.o tree.o
#  endif
#endif


all: $(TARGET)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(DEDUP_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC -o $(TARGET) $(DEDUP_OBJ) $(LIBS)

clean:
	rm -f *~ *.o $(TARGET)

install:
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin/$(TARGET)

