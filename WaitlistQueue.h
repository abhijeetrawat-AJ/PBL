#ifndef WAITLIST_QUEUE_H
#define WAITLIST_QUEUE_H

#include "BookingQueue.h"
#include "LinkedQueue.h"

class WaitlistQueue : public BookingQueue {
private:
    LinkedQueue queue;
    int count;  

public:
    WaitlistQueue() : count(0) {}

    bool insertRequest(const BookingRequest& req) override {
        queue.enqueue(req);  
        count++;
        return true;
    }

    bool getNextRequest(BookingRequest& out) override {
        if (!queue.dequeue(out)) return false;
        count--;
        return true;
    }

    bool peekNextRequest(BookingRequest& out) const override {
        return queue.peek(out);
    }

    bool isEmpty() const override { return queue.isEmpty(); }
    int size() const override { return count; }
    const char* getName() const override { return "Waitlist"; }
};

#endif
