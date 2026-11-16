#ifndef OWNER_APP_H
#define OWNER_APP_H

#include <iostream>
#include <string>
#include "HashMap.h"
#include "MinHeap.h"
#include "LinkedQueue.h"
#include "PrintSpooler.h"
#include "DataManager.h"

using namespace std;

class OwnerApp {
public:
    static void displayMenu() {
        cout << "\n=========================================" << endl;
        cout << "         Owner / Admin Dashboard         " << endl;
        cout << "=========================================" << endl;
        cout << "1. Add New Timetable Slot" << endl;
        cout << "2. Process Next Priority Request" << endl;
        cout << "3. Process Next Waitlist Request" << endl;
        cout << "4. View Complete Timetable Status" << endl;
        cout << "5. View Occupied Slots Only" << endl;
        cout << "6. Execute Pending Print Jobs (Spooler)" << endl;
        cout << "7. Logout" << endl;
        cout << "Enter your choice: ";
    }

    static void run(HashMap& slots, MinHeap& priorityQueue, LinkedQueue& waitlist, PrintSpooler& spooler) {
        int choice = 0;
        string inputBuffer;

        do {
            displayMenu();
            getline(cin, inputBuffer);
            if (!inputBuffer.empty()) {
                try { choice = stoi(inputBuffer); } catch (...) { choice = 0; }
            }

            if (choice == 1) {
                string slotId;
                cout << "Enter new slot ID (e.g., Lab4_Fri_09AM): ";
                getline(cin, slotId);
                
                if (slots.isSlotFree(slotId)) {
                    cout << ">>> Slot already exists in the system." << endl;
                } else {
                    slots.insert(slotId, true);
                    cout << ">>> Slot '" << slotId << "' successfully added and marked as FREE." << endl;
                }
            } 
            else if (choice == 2) {
                if (priorityQueue.isEmpty()) {
                    cout << ">>> No priority requests in queue." << endl;
                } else {
                    BookingRequest nextReq = priorityQueue.peekMin(); // Inspect first
                    cout << "\n>>> Processing Priority Request for " << nextReq.studentId << endl;
                    cout << "    Priority Score: " << -nextReq.priorityFactor << " | Pages: " << nextReq.pageCount << endl;

                    string slotId;
                    cout << "Assign free slot ID to confirm booking (or enter 'cancel'): ";
                    getline(cin, slotId);

                    if (slotId == "cancel" || slotId.empty()) {
                        cout << ">>> Assignment cancelled. Student request retained in queue." << endl;
                    } else if (slots.isSlotFree(slotId)) {
                        priorityQueue.extractMin(); // Extract ONLY after successful slot match
                        slots.setSlotStatus(slotId, false);
                        DataManager::logBookingRecord(nextReq, "PRIORITY");
                        spooler.dispatchToPrinter(nextReq);
                        cout << ">>> Slot " << slotId << " assigned successfully." << endl;
                    } else {
                        cout << ">>> Slot assignment failed (Slot occupied/invalid). Request retained in queue." << endl;
                    }
                }
            }
            else if (choice == 3) {
                if (waitlist.isEmpty()) {
                    cout << ">>> Waitlist is empty." << endl;
                } else {
                    BookingRequest nextReq = waitlist.peek(); // Inspect first
                    cout << "\n>>> Processing Waitlist Request for " << nextReq.studentId << endl;
                    cout << "    Pages: " << nextReq.pageCount << endl;

                    string slotId;
                    cout << "Assign free slot ID to confirm booking (or enter 'cancel'): ";
                    getline(cin, slotId);

                    if (slotId == "cancel" || slotId.empty()) {
                        cout << ">>> Assignment cancelled. Student request retained in waitlist." << endl;
                    } else if (slots.isSlotFree(slotId)) {
                        waitlist.dequeue(); // Dequeue ONLY after successful slot match
                        slots.setSlotStatus(slotId, false);
                        DataManager::logBookingRecord(nextReq, "WAITLIST");
                        spooler.dispatchToPrinter(nextReq);
                        cout << ">>> Slot " << slotId << " assigned successfully." << endl;
                    } else {
                        cout << ">>> Slot assignment failed (Slot occupied/invalid). Request retained in waitlist." << endl;
                    }
                }
            }
            else if (choice == 4) {
                slots.displayAllSlots();
            }
            else if (choice == 5) {
                slots.displayFilledSlots();
            }
            else if (choice == 6) {
                spooler.executeAllJobs();
            }
            else if (choice == 7) {
                cout << "Logging out of Admin Dashboard..." << endl;
            }
            else {
                cout << "Invalid choice. Try again." << endl;
            }
        } while (choice != 7);
    }
};

#endif