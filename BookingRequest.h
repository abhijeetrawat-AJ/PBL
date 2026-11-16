#ifndef BOOKING_REQUEST_H
#define BOOKING_REQUEST_H

#include <iostream>
using namespace std;

class BookingRequest {
private:
    int rollNo;        
    int deadlineDate;  
    int requestNo;
    int pageCount;     
    int slotKey;       

public:
    BookingRequest() {
        rollNo = 0;
        deadlineDate = 99999999;
        requestNo = 0;
        pageCount = 0;
        slotKey = -1;
    }

    BookingRequest(int roll, int deadline, int reqNo, int pages) {
        rollNo = roll;
        deadlineDate = deadline;
        requestNo = reqNo;
        pageCount = pages;
        slotKey = -1;
    }
    bool operator<(const BookingRequest& other) const {
        if (deadlineDate != other.deadlineDate) {
            return deadlineDate < other.deadlineDate;
        }
        return requestNo < other.requestNo;
    }
    int getPrintTimeMinutes() const {
        int minutes = 2 + pageCount / 5;
        if (pageCount % 5 != 0) {
            minutes++;
        }
        return minutes;
    }

    int getRollNo() const { return rollNo; }
    int getDeadlineDate() const { return deadlineDate; }
    int getRequestNo() const { return requestNo; }
    int getPageCount() const { return pageCount; }
    int getSlotKey() const { return slotKey; }

    // Slot alloted save here 
    void setSlotKey(int key) { slotKey = key; }

    // Check if this request has received a slot
    bool hasSlot() const { return slotKey != -1; }

    void print() const {
        cout << "Roll: " << rollNo
             << " | Deadline: " << deadlineDate
             << " | Req: " << requestNo
             << " | Pages: " << pageCount
             << " | Print time: " << getPrintTimeMinutes() << " min" << endl;
    }
};

#endif