#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "LinkedList.h"

class HashMap {
private:
    static const int SIZE = 211;   // prime number
    LinkedList* table;            

    int hashFunction(int key) const {
        return ((key % SIZE) + SIZE) % SIZE;   
    }

public:
    HashMap() { table = new LinkedList[SIZE]; }

    ~HashMap() { delete[] table; }

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

    bool insert(int key, bool isFree) {
        return table[hashFunction(key)].insert(key, isFree);
    }

    bool contains(int key) const {
        return table[hashFunction(key)].contains(key);
    }

    bool isSlotFree(int key) const {
        bool status = false;
        if (table[hashFunction(key)].search(key, status)) return status;
        return false;
    }

    bool setSlotStatus(int key, bool isFree) {
        return table[hashFunction(key)].updateStatus(key, isFree);
    }

    bool remove(int key) {
        return table[hashFunction(key)].remove(key);
    }

    const LinkedList* getTable() const { return table; }
    int getTableSize() const { return SIZE; }
};

#endif