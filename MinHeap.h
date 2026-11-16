#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "BookingRequest.h"

class MinHeap {
private:
    BookingRequest* heapArray;
    int capacity;
    int currentSize;

    void swapItems(int i, int j) {
        BookingRequest temp = heapArray[i];
        heapArray[i] = heapArray[j];
        heapArray[j] = temp;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heapArray[index] < heapArray[parent]) {
                swapItems(index, parent);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        while (true) {
            int smallest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < currentSize && heapArray[left] < heapArray[smallest])
                smallest = left;
            if (right < currentSize && heapArray[right] < heapArray[smallest])
                smallest = right;

            if (smallest == index) break;
            swapItems(index, smallest);
            index = smallest;
        }
    }

public:
    MinHeap(int cap = 50) {
        capacity = cap;
        currentSize = 0;
        heapArray = new BookingRequest[capacity];
    }

    ~MinHeap() { delete[] heapArray; }

    MinHeap(const MinHeap&) = delete;
    MinHeap& operator=(const MinHeap&) = delete;

    bool insert(const BookingRequest& request) {
        if (currentSize == capacity) return false;
        heapArray[currentSize] = request;
        heapifyUp(currentSize);
        currentSize++;
        return true;
    }

    bool peekMin(BookingRequest& out) const {
        if (currentSize == 0) return false;
        out = heapArray[0];
        return true;
    }

    bool extractMin(BookingRequest& out) {
        if (currentSize == 0) return false;
        out = heapArray[0];
        currentSize--;
        if (currentSize > 0) {
            heapArray[0] = heapArray[currentSize];
            heapifyDown(0);
        }
        return true;
    }

    bool isEmpty() const { return currentSize == 0; }
    int getSize() const { return currentSize; }

    const BookingRequest& getAt(int index) const { return heapArray[index]; }
};

#endif