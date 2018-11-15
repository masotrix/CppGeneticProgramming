CC=g++
CFLAGS=-std=c++14 -g -Isrc -Isrc/ArrayGAS
CFLAGS+=`pkg-config --cflags --libs opencv`
MKDIR_P= mkdir -p

GPDEPS= src/GP.h
GPOBJ= src/GP.o
ARRAYSSRC= src/Arrays
ARRAYSOBJ= $(GPOBJ) $(ARRAYSSRC)/testArrays.o
ARRAYSBUILD= build/buildArrays

all: \
	builddir \
	arraysdir \
	$(ARRAYSBUILD)/testArrays



%.o: %.cpp $(GPDEPS)
	$(CC) $< -c -o $@ $(CFLAGS)

builddir:
	${MKDIR_P} build

arraysdir: 
	${MKDIR_P} \
  $(ARRAYSBUILD) \

$(ARRAYSBUILD)/testArrays: $(ARRAYSOBJ)
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean

clean:
	find . -type f -name "*.o" -delete


