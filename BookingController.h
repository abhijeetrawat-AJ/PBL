#ifndef BOOKING_CONTROLLER_H
#define BOOKING_CONTROLLER_H

#include <iostream>
#include <fstream>
#include "LabTimetable.h"
#include "HashMap.h"
#include "AvailabilityChecker.h"
#include "BookingQueue.h"
#include "PriorityBookingQueue.h"
#include "WaitlistQueue.h"
#include "RequestRegistry.h"
#include "BookingFile.h"
using namespace std;

enum SubmitResult {
    SUBMIT_BOOKED,          
    SUBMIT_QUEUED_PRIORITY, 
    SUBMIT_WAITLISTED,      
    SUBMIT_DUPLICATE,       
    SUBMIT_INVALID_ROLL,
    SUBMIT_INVALID_DATE,
    SUBMIT_INVALID_PAGES,   
    SUBMIT_SYSTEM_FULL
};

enum CancelResult {
    CANCEL_BOOKING,
    CANCEL_QUEUED,     
    CANCEL_NOT_FOUND
};

class BookingController {
public:
    static const int PERIOD_MINUTES = 55;

private:
    LabTimetable* labs;        
    int labCount;
    HashMap slots;             
    AvailabilityChecker checker;
    BookingQueue* priorityQueue;   
    BookingQueue* waitlist;
    RequestRegistry registry;
    int nextRequestNo;
    bool autoSaveOn;
    char savePath[128];

    static bool isValidDate(int date) {
        int year = date / 10000;
        int month = (date / 100) % 100;
        int day = date % 100;
        if (year < 2000 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        int maxDay = daysInMonth[month - 1];
        if (month == 2 && leap) maxDay = 29;
        return day >= 1 && day <= maxDay;
    }

    bool findEarliestSlot(int& lab, int& day, int& period) const {
        for (int d = 0; d < LabTimetable::DAYS; d++) {
            if (checker.findFirstAvailable(d, lab, period)) {
                day = d;
                return true;
            }
        }
        return false;
    }

    bool takeLive(BookingQueue* q, RequestState expected, BookingRequest& out) {
        BookingRequest r;
        while (q->getNextRequest(r)) {
            const RegistryEntry* e = registry.find(r.getRollNo());
            if (e != nullptr && e->requestNo == r.getRequestNo() && e->state == expected) {
                out = r;
                return true;
            }
        }
        return false;
    }

    int allocate(int& lastRoll) {
        int assigned = 0;
        lastRoll = -1;
        int lab = 0, day = 0, period = 0;
        while (findEarliestSlot(lab, day, period)) {
            BookingRequest next;
            if (!takeLive(priorityQueue, STATE_PRIORITY_QUEUED, next) &&
                !takeLive(waitlist, STATE_WAITLISTED, next)) {
                break;
            }
            checker.book(lab, day, period);
            RegistryEntry* e = registry.find(next.getRollNo());
            e->state = STATE_CONFIRMED;
            e->slotKey = LabTimetable::makeSlotKey(lab, day, period);
            lastRoll = next.getRollNo();
            assigned++;
        }
        return assigned;
    }

    static void decodeSlotKey(int key, int& lab, int& day, int& period) {
        lab = key / 100;
        day = (key % 100) / 10;
        period = key % 10;
    }

    void printGroup(RequestState state, const char* title) const {
        cout << title << " (" << registry.countState(state) << "):" << endl;
        for (int i = 0; i < registry.size(); i++) {
            const RegistryEntry& e = registry.getAt(i);
            if (e.state != state) continue;
            cout << "  Roll " << e.rollNo;
            if (state == STATE_CONFIRMED) {
                int lab, day, period;
                decodeSlotKey(e.slotKey, lab, day, period);
                cout << " -> Lab " << lab << " (" << labs[lab - 1].getRoomName() << "), "
                     << LabTimetable::dayName(day) << " " << LabTimetable::periodTime(period);
            }
            cout << endl;
        }
    }

    bool restoreRequest(const SavedRequest& s) {
        if (s.rollNo <= 0 || !isValidDate(s.deadlineDate)) return false;
        if (s.state < STATE_PRIORITY_QUEUED || s.state > STATE_CONFIRMED) return false;
        if (s.requestNo < 1 || s.pageCount < 1) return false;
        BookingRequest req(s.rollNo, s.deadlineDate, s.requestNo, s.pageCount);
        if (req.getPrintTimeMinutes() > PERIOD_MINUTES) return false;
        if (registry.find(s.rollNo) != nullptr || registry.isFull()) return false;

        RequestState state = (RequestState)s.state;
        int slotKey = -1;
        if (state == STATE_CONFIRMED) {
            int lab = 0, day = 0, period = 0;
            if (s.slotKey >= 0) decodeSlotKey(s.slotKey, lab, day, period);
            if (s.slotKey >= 0 && checker.book(lab, day, period)) {
                slotKey = s.slotKey;
            } else {
                cout << "[Warning] Saved slot of roll " << s.rollNo
                     << " is not available any more. Re-queued." << endl;
                state = STATE_PRIORITY_QUEUED;
            }
        }

        if (state == STATE_PRIORITY_QUEUED) {
            if (!priorityQueue->insertRequest(req)) {
                waitlist->insertRequest(req);
                state = STATE_WAITLISTED;
            }
        } else if (state == STATE_WAITLISTED) {
            waitlist->insertRequest(req);
        }

        registry.add(s.rollNo, s.requestNo, state, s.deadlineDate, s.pageCount);
        if (slotKey != -1) registry.find(s.rollNo)->slotKey = slotKey;
        if (s.requestNo >= nextRequestNo) nextRequestNo = s.requestNo + 1;
        return true;
    }

    void autoSave() const {
        if (!autoSaveOn) return;
        if (!saveToFile(savePath)) {
            cout << "[Warning] Could not save bookings to " << savePath << endl;
        }
    }

public:
    BookingController(LabTimetable* labArray, int count,
                      int priorityCapacity = 50, int maxActiveRequests = 200)
        : labs(labArray), labCount(count), slots(),
          checker(labArray, count, &slots),
          priorityQueue(nullptr), waitlist(nullptr),
          registry(maxActiveRequests), nextRequestNo(1), autoSaveOn(false) {
        savePath[0] = '\0';
        priorityQueue = new PriorityBookingQueue(priorityCapacity);
        waitlist = new WaitlistQueue();
        checker.buildFromTimetable();
    }

    ~BookingController() {
        delete priorityQueue;   
        delete waitlist;
    }

    BookingController(const BookingController&) = delete;
    BookingController& operator=(const BookingController&) = delete;

    void setAutoSaveFile(const char* filename) {
        int i = 0;
        while (filename[i] != '\0' && i < 127) {
            savePath[i] = filename[i];
            i++;
        }
        savePath[i] = '\0';
        autoSaveOn = (i > 0);
    }

    bool saveToFile(const char* filename) const {
        ofstream file(filename);
        if (!file.is_open()) return false;
        file << "# roll,requestNo,state(1=priority 2=waitlist 3=booked),slotKey,deadline,pages\n";

        int lastNo = 0;
        for (int written = 0; written < registry.size(); written++) {
            int best = -1;
            for (int i = 0; i < registry.size(); i++) {
                int no = registry.getAt(i).requestNo;
                if (no > lastNo && (best == -1 || no < registry.getAt(best).requestNo)) best = i;
            }
            if (best == -1) break;
            const RegistryEntry& e = registry.getAt(best);
            SavedRequest s;
            s.rollNo = e.rollNo;
            s.requestNo = e.requestNo;
            s.state = (int)e.state;
            s.slotKey = e.slotKey;
            s.deadlineDate = e.deadlineDate;
            s.pageCount = e.pageCount;
            BookingFile::writeLine(file, s);
            lastNo = e.requestNo;
        }
        file.close();
        return true;
    }

    int loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) return 0;

        char line[128];
        int lineNo = 0;
        int restored = 0;
        while (file.getline(line, 128)) {
            lineNo++;
            if (line[0] == '\0' || line[0] == '#' || line[0] == '\r') continue;
            SavedRequest s;
            if (!BookingFile::parseLine(line, s) || !restoreRequest(s)) {
                cout << "[Warning] " << filename << " line " << lineNo << " skipped (bad data)" << endl;
                continue;
            }
            restored++;
        }
        file.close();

        int lastRoll;
        allocate(lastRoll);
        return restored;
    }

    SubmitResult submitRequest(int rollNo, int deadlineDate, int pages) {
        if (rollNo <= 0) return SUBMIT_INVALID_ROLL;
        if (!isValidDate(deadlineDate)) return SUBMIT_INVALID_DATE;

        BookingRequest req(rollNo, deadlineDate, nextRequestNo, pages);
        if (pages < 1 || req.getPrintTimeMinutes() > PERIOD_MINUTES)
            return SUBMIT_INVALID_PAGES;
        if (registry.find(rollNo) != nullptr) return SUBMIT_DUPLICATE;
        if (registry.isFull()) return SUBMIT_SYSTEM_FULL;

        RequestState state = STATE_PRIORITY_QUEUED;
        if (!priorityQueue->insertRequest(req)) {   
            waitlist->insertRequest(req);
            state = STATE_WAITLISTED;
        }
        registry.add(rollNo, nextRequestNo, state, deadlineDate, pages);
        nextRequestNo++;

        int lastRoll;
        allocate(lastRoll);   
        autoSave();           

        const RegistryEntry* e = registry.find(rollNo);
        if (e->state == STATE_CONFIRMED) return SUBMIT_BOOKED;
        if (e->state == STATE_WAITLISTED) return SUBMIT_WAITLISTED;
        return SUBMIT_QUEUED_PRIORITY;
    }

    CancelResult cancelRequest(int rollNo, int& reassignedRoll) {
        reassignedRoll = -1;
        RegistryEntry* e = registry.find(rollNo);
        if (e == nullptr) return CANCEL_NOT_FOUND;

        if (e->state == STATE_CONFIRMED) {
            int lab, day, period;
            decodeSlotKey(e->slotKey, lab, day, period);
            checker.release(lab, day, period);
            registry.remove(rollNo);
            allocate(reassignedRoll);  
            autoSave();
            return CANCEL_BOOKING;
        }

        registry.remove(rollNo);  
        autoSave();
        return CANCEL_QUEUED;
    }

    bool getBooking(int rollNo, int& labNo, int& day, int& period) const {
        const RegistryEntry* e = registry.find(rollNo);
        if (e == nullptr || e->state != STATE_CONFIRMED) return false;
        decodeSlotKey(e->slotKey, labNo, day, period);
        return true;
    }

    void printStatus(int rollNo) const {
        cout << "Roll " << rollNo << ": ";
        const RegistryEntry* e = registry.find(rollNo);
        if (e == nullptr) {
            cout << "no active request" << endl;
        } else if (e->state == STATE_CONFIRMED) {
            int lab, day, period;
            decodeSlotKey(e->slotKey, lab, day, period);
            cout << "BOOKED - Lab " << lab << " (" << labs[lab - 1].getRoomName() << "), "
                 << LabTimetable::dayName(day) << " " << LabTimetable::periodTime(period) << endl;
        } else if (e->state == STATE_PRIORITY_QUEUED) {
            cout << "waiting in priority queue" << endl;
        } else {
            cout << "on waitlist" << endl;
        }
    }

    bool hasActiveRequest(int rollNo) const { return registry.find(rollNo) != nullptr; }

    int confirmedCount() const { return registry.countState(STATE_CONFIRMED); }
    int priorityWaitingCount() const { return registry.countState(STATE_PRIORITY_QUEUED); }
    int waitlistedCount() const { return registry.countState(STATE_WAITLISTED); }

    int availableOnDay(int day) const { return checker.countAvailableOnDay(day); }
    void printAvailableOnDay(int day) const { checker.printAvailableOnDay(day); }

    int getLabCount() const { return labCount; }

    void printLabTimetable(int labNo) const {
        if (labNo < 1 || labNo > labCount || !labs[labNo - 1].isLoaded()) {
            cout << "Invalid lab number." << endl;
            return;
        }
        labs[labNo - 1].print();
    }

    void printAllRequests() const {
        printGroup(STATE_CONFIRMED, "Confirmed bookings");
        printGroup(STATE_PRIORITY_QUEUED, "Waiting in priority queue");
        printGroup(STATE_WAITLISTED, "On waitlist");
    }
};

#endif
