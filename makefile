IDIR =include
LIB=lib
ODIR=bin
SRC=src
TEST=test
LIB_NAME=IKeyValueStore

CXX=g++
CXXFLAGS=-g -Wall -std=c++17 -I $(IDIR) -pthread


$(TEST)/client: $(TEST)/client.cpp $(LIB)/lib$(LIB_NAME).so
	$(CXX) $(CXXFLAGS) -L $(LIB) -l$(LIB_NAME) $<  -o ./$(TEST)/client
	clang-format -i */*.cpp */*.h

$(LIB)/lib$(LIB_NAME).so: $(SRC)/KeyValueStore.cpp $(SRC)/IKeyValueStore.cpp
	mkdir -p $(LIB)
	$(CXX) $(CXXFLAGS) $^ -fPIC -shared -Wl,-soname,lib$(LIB_NAME).so -o $@

.PHONY: clean run
clean:
	rm -f $(ODIR)/*.o $(TEST)/*.o $(LIB)/*.so $(TEST)/client db/*