/**
        IKeyValueStore.cpp
        Purpose: Gives an interface to the user with limited access to the
   key-value store.
        @author Halil Bengu
*/

#include "IKeyValueStore.h"
#include "KeyValueStore.h"

#include <iostream>
#include <unistd.h>

/*
  Saves the given Key Value pair to store
        @param key, value
        @return 0 for success, 1 if key already exists
*/
int IKeyValueStore::put(std::string key, std::string value) {
  int returnVal;

  storeMutex.lock();
  returnVal = kvStore.put(key, value);
  storeMutex.unlock();

  return returnVal;
}

/*
  Remove the given key from store
        @param key
        @return 0 for success, 1 if key not found
*/
int IKeyValueStore::remove(std::string key) {
  int returnVal;

  storeMutex.lock();
  returnVal = kvStore.remove(key);
  storeMutex.unlock();

  return returnVal;
}

/*
  Returns the value for the give key
        @param key
        @return value
*/
std::string IKeyValueStore::get(std::string key) {
  std::string returnStr;

  storeMutex.lock();
  returnStr = kvStore.get(key);
  storeMutex.unlock();

  return returnStr;
}

/*
  Prints out information about store for debug
        @param none
        @return none
*/
void IKeyValueStore::info() {
  std::cout << "Number of elements in Memory: " << kvStore.inMemoryStore.size()
            << std::endl;
  std::cout << "Bucket Count: " << kvStore.inMemoryStore.bucket_count()
            << std::endl;
  std::cout << "In Memory Amount (Bytes): " << kvStore.inMemoryAmount
            << std::endl;
  std::cout << "LRU List Size: " << kvStore.lruList.size() << std::endl;
}

/*
  Prints out sample portion of data that saved to disk
        @param none
        @return none
*/
void IKeyValueStore::readSampleMap(int i) { kvStore.readMap(i); }