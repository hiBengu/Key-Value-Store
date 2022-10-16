#ifndef IKEYVALUESTORE_H
#define IKEYVALUESTORE_H

#include "KeyValueStore.h"
#include <mutex>
#include <string>

class IKeyValueStore {
private:
  IKeyValueStore *instance;
  KeyValueStore kvStore;
  std::mutex storeMutex;

public:
  void info();
  void readSampleMap(int);

  int put(std::string, std::string);
  int remove(std::string);
  std::string get(std::string);
};

#endif