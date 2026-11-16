#ifndef PRIORITY_BOOKING_QUEUE_H
#define PRIORITY_BOOKING_QUEUE_H

#include "BookingQueue.h"
#include "MinHeap.h"

class PriorityBookingQueue : public BookingQueue {
private:
    MinHeap heap;   

public:
    PriorityBookingQueue(int capacity = 50) : heap(capacity) {}

    bool insertRequest(const BookingRequest& req) override {
        return heap.insert(req);
    }

    bool getNextRequest(BookingRequest& out) override {
        return heap.extractMin(out);
    }

    bool peekNextRequest(BookingRequest& out) const override {
        return heap.peekMin(out);
    }

    bool isEmpty() const override { return heap.isEmpty(); }
    int size() const override { return heap.getSize(); }
    const char* getName() const override { return "PriorityQueue"; }
};

#endif
