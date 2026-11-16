#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "HashMap.h"
#include "MinHeap.h"
#include "LinkedQueue.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

class DataManager {
public:
    static void loadTimetable(HashMap& slots, const string& filename = "timetable.csv") {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[System Info] Defaulting timetable slots..." << endl;
            slots.insert("Lab5_Mon_10AM", false); 
            slots.insert("Lab5_Mon_11AM", true);  
            slots.insert("Lab2_Tue_09AM", true);  
            slots.insert("Lab2_Tue_10AM", false); 
            slots.insert("Lab3_Wed_02PM", true);  
            slots.insert("Lab1_Thu_11AM", false); 
            saveTimetable(slots, filename);
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string slotId, statusStr;
            if (getline(ss, slotId, ',') && getline(ss, statusStr, ',')) {
                bool isFree = (statusStr == "FREE" || statusStr == "1");
                slots.insert(slotId, isFree);
            }
        }
        file.close();
        cout << "[System] Timetable successfully loaded." << endl;
    }

    static void saveTimetable(HashMap& slots, const string& filename = "timetable.csv") {
        ofstream file(filename);
        if (!file.is_open()) return;

        LinkedList* table = slots.getTable();
        for (int i = 0; i < slots.getTableSize(); i++) {
            HashNode* curr = table[i].getHead();
            while (curr != nullptr) {
                file << curr->key << "," << (curr->isFree ? "FREE" : "OCCUPIED") << "\n";
                curr = curr->next;
            }
        }
        file.close();
    }

    // PQ File Handling
    static void loadPriorityQueue(MinHeap& pq, const string& filename = "priority_queue.csv") {
        ifstream file(filename);
        if (!file.is_open()) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string id, daysStr, yearStr, pagesStr;
            if (getline(ss, id, ',') && getline(ss, daysStr, ',') && 
                getline(ss, yearStr, ',') && getline(ss, pagesStr, ',')) {
                BookingRequest req(id, stoi(daysStr), stoi(yearStr), stoi(pagesStr));
                req.priorityFactor = -req.priorityFactor; // Re-invert to simulate max-heap extraction
                pq.insert(req);
            }
        }
        file.close();
        cout << "[System] Priority Queue state loaded." << endl;
    }

    static void savePriorityQueue(const MinHeap& pq, const string& filename = "priority_queue.csv") {
        ofstream file(filename);
        if (!file.is_open()) return;
        for (int i = 0; i < pq.getSize(); i++) {
            BookingRequest req = pq.getAt(i);
            file << req.studentId << "," << req.deadlineDays << "," 
                 << req.academicYear << "," << req.pageCount << "\n";
        }
        file.close();
    }

    // Waitlist File Handling
    static void loadWaitlist(LinkedQueue& wl, const string& filename = "waitlist.csv") {
        ifstream file(filename);
        if (!file.is_open()) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string id, daysStr, yearStr, pagesStr;
            if (getline(ss, id, ',') && getline(ss, daysStr, ',') && 
                getline(ss, yearStr, ',') && getline(ss, pagesStr, ',')) {
                BookingRequest req(id, stoi(daysStr), stoi(yearStr), stoi(pagesStr));
                wl.enqueue(req);
            }
        }
        file.close();
        cout << "[System] Waitlist state loaded." << endl;
    }

    static void saveWaitlist(const LinkedQueue& wl, const string& filename = "waitlist.csv") {
        ofstream file(filename);
        if (!file.is_open()) return;
        QueueNode* curr = wl.getFront();
        while (curr != nullptr) {
            BookingRequest req = curr->data;
            file << req.studentId << "," << req.deadlineDays << "," 
                 << req.academicYear << "," << req.pageCount << "\n";
            curr = curr->next;
        }
        file.close();
    }

    static void logBookingRecord(const BookingRequest& req, const string& type, const string& filename = "booking_records.csv") {
        ofstream file(filename, ios::app);
        if (!file.is_open()) return;
        file << req.studentId << "," << type << "," << req.academicYear << ","
             << req.deadlineDays << "," << req.pageCount << "," << req.printTimeMinutes << "\n";
        file.close();
    }
};

#endif