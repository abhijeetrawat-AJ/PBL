#ifndef STUDENT_APP_H
#define STUDENT_APP_H

#include <iostream>
#include <string>
#include "HashMap.h"
#include "MinHeap.h"
#include "LinkedQueue.h"
#include "StudentDatabase.h"

using namespace std;

class StudentApp {
public:
    static void displayMenu() {
        cout << "\n=========================================" << endl;
        cout << "           Student Booking Portal        " << endl;
        cout << "=========================================" << endl;
        cout << "1. Check Slot Availability" << endl;
        cout << "2. Request Priority Booking" << endl;
        cout << "3. Join General Waitlist" << endl;
        cout << "4. Logout" << endl;
        cout << "Enter your choice: ";
    }

    static void run(HashMap& slots, MinHeap& priorityQueue, LinkedQueue& waitlist, StudentDatabase& studentDb) {
        int choice = 0;
        string inputBuffer;

        do {
            displayMenu();
            getline(cin, inputBuffer);
            if (!inputBuffer.empty()) {
                try { choice = stoi(inputBuffer); } catch (...) { choice = 0; }
            }

            if (choice == 1) {
                string slot;
                cout << "Enter slot ID to check (e.g., Lab5_Mon_11AM): ";
                getline(cin, slot);
                if (slots.isSlotFree(slot)) {
                    cout << ">>> Status: FREE. Slot is available for booking!" << endl;
                } else {
                    cout << ">>> Status: OCCUPIED/NOT FOUND. Please select another slot." << endl;
                }
            } 
            else if (choice == 2) {
                string studentId, pin, studentName;
                int year = 1, days = 0, pages = 0;

                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter PIN (Authentication): ";
                getline(cin, pin);

                if (!studentDb.authenticate(studentId, pin, year, studentName)) {
                    cout << ">>> Priority Booking aborted." << endl;
                    continue;
                }

                cout << ">>> Welcome, " << studentName << " (Year " << year << ")" << endl;
                cout << "Enter days until submission deadline: ";
                getline(cin, inputBuffer);
                if (!inputBuffer.empty()) days = stoi(inputBuffer);

                cout << "Enter total pages to print: ";
                getline(cin, inputBuffer);
                if (!inputBuffer.empty()) pages = stoi(inputBuffer);

                BookingRequest req(studentId, days, year, pages);
                req.priorityFactor = -req.priorityFactor; // Max-Heap simulation
                priorityQueue.insert(req);

                cout << ">>> Priority Request logged! (Est. Print Time: " << req.printTimeMinutes << " min)" << endl;
            }
            else if (choice == 3) {
                string studentId, pin, studentName;
                int year = 1, pages = 0;

                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter PIN (Authentication): ";
                getline(cin, pin);

                if (!studentDb.authenticate(studentId, pin, year, studentName)) {
                    cout << ">>> Waitlist Booking aborted." << endl;
                    continue;
                }

                cout << ">>> Welcome, " << studentName << endl;
                cout << "Enter total pages to print: ";
                getline(cin, inputBuffer);
                if (!inputBuffer.empty()) pages = stoi(inputBuffer);

                BookingRequest req(studentId, 999, year, pages);
                waitlist.enqueue(req);

                cout << ">>> " << studentId << " added to general waitlist. (Est. Time: " << req.printTimeMinutes << " min)" << endl;
            }
            else if (choice == 4) {
                cout << "Logging out of Student Portal..." << endl;
            }
            else {
                cout << "Invalid choice. Try again." << endl;
            }
        } while (choice != 4);
    }
};

#endif