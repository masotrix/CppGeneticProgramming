CC=g++
CFLAGS=-std=c++14 -g -Isrc -Isrc/Arrays -Isrc/AST -Isrc/Plot
CFLAGS+=`pkg-config --cflags --libs opencv` -Isrc/Arrays/TSP
MKDIR_P= mkdir -p

ARRAYSSRC= src/Arrays
TSPSRC= src/Arrays/TSP
ASTSRC= src/AST

ASTBUILD= build/AST
ARRAYSBUILD= build/Arrays

GPDEPS= src/GP.h src/Plot/Plot.h src/Arrays/TSP/TSP.h
PLOTOBJ= src/Plot/Plot.o
ASTOBJ= $(ASTSRC)/AST.o
GPOBJ= $(ASTOBJ) src/GP.o
ASTTESTOBJ= $(ASTOBJ) $(ASTSRC)/testAst.o
ARRAYSOBJ= $(GPOBJ) $(ARRAYSSRC)/testArrays.o
GPTESTSTASKTWOOBJ= $(GPOBJ) $(PLOTOBJ)  \
							$(TSPSRC)/hull.o $(TSPSRC)/mst.o\
							$(ARRAYSSRC)/testsTask2.o
GPTASKTWOOBJ= $(GPOBJ) $(PLOTOBJ)  \
							$(TSPSRC)/hull.o $(TSPSRC)/mst.o\
							$(ARRAYSSRC)/task2.o

all: \
	builddir \
	astdir \
	$(ASTBUILD)/testAst \
	arraysdir \
	$(ARRAYSBUILD)/testArrays \
	$(ARRAYSBUILD)/task2 \
	$(ARRAYSBUILD)/testsTask2


%.o: %.cpp $(GPDEPS)
	$(CC) $< -c -o $@ $(CFLAGS)


builddir:
	${MKDIR_P} build

astdir: 
	${MKDIR_P} \
  $(ASTBUILD)
$(ASTBUILD)/testAst: $(ASTTESTOBJ)
	$(CC) $^ -o $@ $(CFLAGS)

arraysdir: 
	${MKDIR_P} \
  $(ARRAYSBUILD)
$(ARRAYSBUILD)/testArrays: $(ARRAYSOBJ)
	$(CC) $^ -o $@ $(CFLAGS)
$(ARRAYSBUILD)/task2: $(GPTASKTWOOBJ)
	$(CC) $^ -o $@ $(CFLAGS)
$(ARRAYSBUILD)/testsTask2: $(GPTESTSTASKTWOOBJ)
	$(CC) $^ -o $@ $(CFLAGS)


.PHONY: clean

clean:
	find . -type f -name "*.o" -delete


