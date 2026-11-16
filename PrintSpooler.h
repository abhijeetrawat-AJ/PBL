#ifndef PRINT_SPOOLER_H
#define PRINT_SPOOLER_H

#include "LinkedQueue.h"
#include <iostream>

using namespace std;

class PrintSpooler {
private:
    LinkedQueue printQueue;
    int totalJobsProcessed;

public:
    PrintSpooler() : totalJobsProcessed(0) {}

    void dispatchToPrinter(BookingRequest req) {
        printQueue.enqueue(req);
        cout << "[Spooler] Job for " << req.studentId << " dispatched to printer queue (" 
             << req.pageCount << " pages)." << endl;
    }

    void executeAllJobs() {
        if (printQueue.isEmpty()) {
            cout << "[Spooler] Printer queue is currently empty." << endl;
            return;
        }

        cout << "\n--- Executing Print Jobs ---" << endl;
        while (!printQueue.isEmpty()) {
            BookingRequest req = printQueue.dequeue();
            cout << " -> Printing " << req.pageCount << " pages for " << req.studentId 
                 << " (Est. Time: " << req.printTimeMinutes << " mins)... DONE." << endl;
            totalJobsProcessed++;
        }
        cout << "----------------------------" << endl;
    }
    
    int getProcessedCount() const {
        return totalJobsProcessed;
    }
};

#endif