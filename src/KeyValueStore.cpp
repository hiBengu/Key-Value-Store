#include "KeyValueStore.h"

#include <filesystem>
#include <iostream>
#include <unistd.h>

KeyValueStore::KeyValueStore() { std::filesystem::create_directory("db"); }

/*
  Add given key value pair to inMemoryStore.
  Checks the memory limit after string input.
  New key is pushed front to LRU list.
        @param key, value
        @return 0 for success, 1 for already exists
*/
int KeyValueStore::put(std::string key, std::string value) {
  auto ret = inMemoryStore.emplace(key, value);
  if (!ret.second)
    return 1;

  addCheckMemory(key.size() + value.size());

  lruList.push_front(key);

  return 0;
}

/*
  Try to remove the given key from inMemoryStore.
  If not exist in memory, try to find the key in disk.
  After removing, substracts the size of the key value pair from calculation.
        @param key
        @return 0 for success, 1 if key doesn't exist
*/
int KeyValueStore::remove(std::string key) {
  std::string foundVal;

  try {
    foundVal = inMemoryStore.at(key);
  } catch (...) {
    foundVal = getDB(key);
    if (foundVal != "") {
      removeCheckMemory(key.size() + foundVal.size());
      return 0;
    } else {
      return 1;
    }
  }

  removeCheckMemory(key.size() + foundVal.size());

  inMemoryStore.erase(key);
  lruList.remove(key);

  return 0;
}

/*
  Returns the value for given key.
  Checks in memory store for key, if found key is updated to front of LRU list.
  If not exists, checks drive,
  If found move pair to in memory store, and front of LRU list
        @param key
        @return key OR "Not Found!"
*/
std::string KeyValueStore::get(std::string key) {
  std::string foundVal;

  try {
    foundVal = inMemoryStore.at(key);
    lruList.remove(key);
    lruList.push_front(key);

    return foundVal;
  } catch (...) {
    foundVal = getDB(key);
    if (foundVal != "") {
      lruList.push_front(key);
      inMemoryStore.emplace(key, foundVal);
      addCheckMemory(key.size() + foundVal.size());
      return foundVal;
    } else {
      return "Not Found!";
    }
  }
}
/*
  Checks if with new pair, memory limit exceeds.
  If limit exceeds, move some part of the unordered map to disk.

  MEMORY CALCULATION:
  (umap.size() * (sizeof(M::value_type) + sizeof(void*)) +
  (umap.bucket_count() * (sizeof(void*) + sizeof(size_t)))
  * 1.5 // estimated allocation overheads

  -- SIMPLIFICATION
  (umap.size() * (sizeof(M::value_type) + 8) +
  (smallest_prime_of_size * (8 + 8))
  * 1.5 // estimated allocation overheads

  -- SIMPLIFICATION
  memorySize = memorySize + (newStringSize + 8) + (16) + (newStringSize(list))

        @param keyValsize total length of key and value
        @return none
*/
void KeyValueStore::addCheckMemory(int keyValSize) {
  inMemoryAmount =
      inMemoryAmount + ((keyValSize + 8) + (16) + keyValSize) * 1.5;

  if (inMemoryAmount >
      MAX_IN_MEMORY_AMOUNT * (1 - MEMORY_TOLERANCE_PERCENTAGE)) {
    freeMemory();
  }
}

/*
  Extracts the size of the removed key value pair from in memory calculation.
        @param keyValSize
        @return none
*/
void KeyValueStore::removeCheckMemory(int keyValSize) {
  inMemoryAmount =
      inMemoryAmount - ((keyValSize + 8) + (16) + keyValSize) * 1.5;
}

/*
  Moves MEMORY_CLEAN_PERCENTAGE of data on memory to disk.
  Hashes the given string between 0-19 to reach the data faster and saves the
  corresponding file.
        @param none
        @return none
*/
void KeyValueStore::freeMemory() {
  char eos = '\0';
  int removeCount = inMemoryStore.size() * MEMORY_CLEAN_PERCENTAGE;

  for (int i = 0; i < removeCount; i++) {
    std::string lastKey = lruList.back();
    lruList.pop_back();

    std::string foundVal = inMemoryStore.at(lastKey);
    removeCheckMemory(lastKey.size() + foundVal.size());

    // Check if the corresponding file exists
    std::string fileName = "./db/map" + std::to_string(hashString(lastKey));
    FILE *file = fopen(fileName.c_str(), "a+");
    fwrite(lastKey.c_str(), 1, lastKey.size(), file);
    fwrite(&eos, 1, 1, file);
    fwrite(foundVal.c_str(), 1, foundVal.size(), file);
    fwrite(&eos, 1, 1, file);
    fclose(file);

    inMemoryStore.erase(lastKey);
  }
}

/*
  Find the corresponding file for given key.
  Find the matching key and value pair.
  After finding the key, start writing the rest of the file
    from the position of found key
        @param key
        @return val
*/
std::string KeyValueStore::getDB(std::string searchKey) {
  bool findKey = true;
  bool getVal = false;
  std::string key = "";
  std::string val = "";
  fpos_t removePosition;
  char ch;

  std::string fileName = "./db/map" + std::to_string(hashString(searchKey));
  FILE *file = fopen(fileName.c_str(), "r+");

  fgetpos(file, &removePosition);
  while (fread(&ch, 1, 1, file)) {

    if (findKey) {
      // Builds the saved key char by char
      if (ch != '\0') {
        key += ch;

      } else if (ch == '\0') {
        if (key == searchKey) {
          getVal = true;
        }

        findKey = false;
        key = "";
      }
    } else if (getVal) {
      // Builds the saved val char bu char
      val += ch;

      if (ch == '\0') {
        char overwriteChar;
        FILE *overwriteFile = fopen(fileName.c_str(), "r+");
        fsetpos(overwriteFile, &removePosition);

        while (fread(&overwriteChar, 1, 1, file)) {
          fwrite(&overwriteChar, 1, 1, overwriteFile);
        }
        // TODO: Clear the end of file

        fclose(overwriteFile);
        fclose(file);
        return val;
      }
    } else {
      if (ch == '\0') {
        fgetpos(file, &removePosition);
        findKey = true;
      }
    }
  }
  fclose(file);
  return val;
}

/*
  Hashes string by summing char values and using mod
        @param str
        @return hash
*/
int KeyValueStore::hashString(std::string str) {
  unsigned int h = 0;

  for (size_t i = 0; i < str.length(); i++) {
    h += str[i];
  }

  return h % 20;
}

/*
  HReads the desired map file
        @param int
        @return none
*/
void KeyValueStore::readMap(int i) {
  std::string str = "";
  bool key = true;
  int mapSize = 0;

  char ch;

  std::string fileName = "./db/map" + std::to_string(i);
  FILE *file = fopen(fileName.c_str(), "r+");

  while (fread(&ch, 1, 1, file)) {
    if (ch != '\0') {
      str += ch;

    } else if (ch == '\0') {
      if (key) {
        std::cout << str << " : ";
        str = "";
        key = false;
      } else {
        std::cout << str << std::endl;
        mapSize++;
        key = true;
        str = "";
      }
    }
  }
  std::cout << "################" << std::endl;
  std::cout << "\nMap Size: " << mapSize << "\n" << std::endl;
  fclose(file);
}
