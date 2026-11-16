#ifndef BOOKING_QUEUE_H
#define BOOKING_QUEUE_H

#include "BookingRequest.h"
class BookingQueue {
public:
    BookingQueue() {}

    virtual ~BookingQueue() {}

    BookingQueue(const BookingQueue&) = delete;
    BookingQueue& operator=(const BookingQueue&) = delete;

    virtual bool insertRequest(const BookingRequest& req) = 0;

    virtual bool getNextRequest(BookingRequest& out) = 0;

    virtual bool peekNextRequest(BookingRequest& out) const = 0;

    virtual bool isEmpty() const = 0;
    virtual int size() const = 0;

    virtual const char* getName() const = 0;
};

#endif
