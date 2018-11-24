CC= g++
CFLAGS= -std=c++11 -g

run: main.o
	$(CC) $^ -o $@ $(CFLAGS)

%.o: %.cpp
	$(CC) $< -c -o $@ $(CFLAGS)
