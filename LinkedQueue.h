#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#include "BookingRequest.h"

struct QueueNode {
    BookingRequest data;
    QueueNode* next;
    QueueNode(const BookingRequest& d) {
        data = d;
        next = nullptr;
    }
};

class LinkedQueue {
private:
    QueueNode* front;
    QueueNode* rear;

public:
    LinkedQueue() {
        front = nullptr;
        rear = nullptr;
    }

    ~LinkedQueue() {
        while (front != nullptr) {
            QueueNode* temp = front;
            front = front->next;
            delete temp;
        }
    }

    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;

    void enqueue(const BookingRequest& request) {
        QueueNode* newNode = new QueueNode(request);
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    bool peek(BookingRequest& out) const {
        if (front == nullptr) return false;
        out = front->data;
        return true;
    }

    bool dequeue(BookingRequest& out) {
        if (front == nullptr) return false;
        QueueNode* temp = front;
        out = temp->data;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
        return true;
    }

    bool isEmpty() const { return front == nullptr; }

    QueueNode* getFront() const { return front; }
};

#endif