#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

#include <list>
#include <string>
#include <unordered_map>

/* GLOBAL VARIABLES FOR STORE SPECS */
#define MAX_IN_MEMORY_AMOUNT 10000000
#define MEMORY_TOLERANCE_PERCENTAGE 0.05
#define MEMORY_CLEAN_PERCENTAGE 0.33

class KeyValueStore {
public:
  std::unordered_map<std::string, std::string> inMemoryStore;
  std::list<std::string> lruList;
  int inMemoryAmount = 0;

  KeyValueStore();

  std::string get(std::string);
  std::string getDB(std::string);
  int put(std::string, std::string);
  int remove(std::string);
  int hashString(std::string);
  void addCheckMemory(int);
  void removeCheckMemory(int);
  void freeMemory();
  void readMap(int);
};

#endif