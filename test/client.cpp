#include "IKeyValueStore.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string.h>
#include <thread>
#include <vector>

void threadFunc(IKeyValueStore *kvStore) {
  kvStore->put(std::to_string(rand() % 100), std::to_string(rand() % 100));
}

/*
  PUT random numbers between 0 and 1 billion
        @param amount of input
        @return none
*/
void putMilBatch(IKeyValueStore *kvStore, int num) {
  for (int i = 0; i < num; i++) {
    int num = rand() % 100000000000;
    kvStore->put(std::to_string(num), std::to_string(num));
  }
  std::cout << "PUT Batch Done!" << std::endl;
}

/*
  Handle the Keyboard INPUT, Matches the commands with Library functions
        @param IKeyValueStore, input
        @return none
*/
void handleInput(IKeyValueStore *kvStore, std::string input) {
  std::istringstream iss(input);
  std::vector<std::string> inputs;
  std::string s;

  while (getline(iss, s, ' ')) {
    inputs.push_back(s);
  }

  if (inputs.size() > 3) {
    std::cout << "Too many arguments!" << std::endl;
    return;
  }
  auto t_start = std::chrono::high_resolution_clock::now();
  if (inputs[0] == "GET") {
    std::cout << "Return Value: " << kvStore->get(inputs[1]) << std::endl;
  } else if (inputs[0] == "PUT") {
    std::cout << kvStore->put(inputs[1], inputs[2]) << std::endl;
  } else if (inputs[0] == "DELETE") {
    std::cout << kvStore->remove(inputs[1]) << std::endl;
  } else if (inputs[0] == "INFO") {
    kvStore->info();
  } else if (inputs[0] == "TEST0") { // TEST THREADING
    std::thread thread0(threadFunc, kvStore);
    std::thread thread1(threadFunc, kvStore);
    std::thread thread2(threadFunc, kvStore);
    std::thread thread3(threadFunc, kvStore);

    thread0.join();
    thread1.join();
    thread2.join();
    thread3.join();
  } else if (inputs[0] == "BATCH") {
    putMilBatch(kvStore, stoi(inputs[1]));
  } else if (inputs[0] == "READMAP") {
    kvStore->readSampleMap(stoi(inputs[1]));
  } else {
    std::cout << "Wrong Input!" << std::endl;
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  double elapsed_time_ms =
      std::chrono::duration<double, std::milli>(t_end - t_start).count();
  std::cout << "Execution time: " << elapsed_time_ms << "\n" << std::endl;
}

/*
  Waits command from keyboard input
        @param none
        @return none
*/
int main() {
  IKeyValueStore *kvStore = new IKeyValueStore();

  std::string input;
  while (true) {
    std::cout << "Please Enter Command: ";
    getline(std::cin, input);

    handleInput(kvStore, input);
  }
}