IDIR =include
LIB=lib
ODIR=bin
SRC=src
TEST=test
LIB_NAME=IKeyValueStore

CXX=g++
CXXFLAGS=-g -Wall -std=c++17 -I $(IDIR) -pthread


./test_client: $(TEST)/client.cpp $(SRC)/KeyValueStore.cpp $(SRC)/IKeyValueStore.cpp
	$(CXX) $(CXXFLAGS) $^  -o ./test_client
	clang-format -i */*.cpp */*.h

.PHONY: clean run
clean:
	rm -f $(ODIR)/*.o $(TEST)/*.o $(LIB)/*.so test_client db/*