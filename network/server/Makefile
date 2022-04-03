CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
BIN=run_server
SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^ -lpthread
%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)
