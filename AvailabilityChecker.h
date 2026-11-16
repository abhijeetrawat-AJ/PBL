#ifndef AVAILABILITY_CHECKER_H
#define AVAILABILITY_CHECKER_H

#include <iostream>
#include "LabTimetable.h"
#include "HashMap.h"
using namespace std;

class AvailabilityChecker {
private:
    LabTimetable* labs;
    int labCount;
    HashMap* slots;

    bool validSlot(int labNo, int day, int period) const {
        if (labNo < 1 || labNo > labCount) return false;
        if (!labs[labNo - 1].isLoaded()) return false;
        if (day < 0 || day >= LabTimetable::DAYS) return false;
        if (period < 0 || period >= LabTimetable::PERIODS) return false;
        return true;
    }

public:
    AvailabilityChecker(LabTimetable* labArray, int count, HashMap* map) {
        labs = labArray;
        labCount = count;
        slots = map;
    }

    int buildFromTimetable() {
        int added = 0;
        for (int i = 0; i < labCount; i++) {
            if (!labs[i].isLoaded()) continue;
            for (int d = 0; d < LabTimetable::DAYS; d++) {
                for (int p = 0; p < LabTimetable::PERIODS; p++) {
                    if (labs[i].isFree(d, p)) {
                        int key = LabTimetable::makeSlotKey(labs[i].getLabNo(), d, p);
                        if (slots->insert(key, true)) added++;
                    }
                }
            }
        }
        return added;
    }

    bool isAvailable(int labNo, int day, int period) const {
        if (!validSlot(labNo, day, period)) return false;
        int key = LabTimetable::makeSlotKey(labNo, day, period);
        return slots->contains(key) && slots->isSlotFree(key);
    }

    bool book(int labNo, int day, int period) {
        if (!isAvailable(labNo, day, period)) return false;
        int key = LabTimetable::makeSlotKey(labNo, day, period);
        return slots->setSlotStatus(key, false);
    }

    bool release(int labNo, int day, int period) {
        if (!validSlot(labNo, day, period)) return false;
        int key = LabTimetable::makeSlotKey(labNo, day, period);
        if (!slots->contains(key)) return false;
        if (slots->isSlotFree(key)) return false;
        return slots->setSlotStatus(key, true);
    }

    int countAvailableOnDay(int day) const {
        int count = 0;
        for (int lab = 1; lab <= labCount; lab++)
            for (int p = 0; p < LabTimetable::PERIODS; p++)
                if (isAvailable(lab, day, p)) count++;
        return count;
    }

    void printAvailableOnDay(int day) const {
        cout << "Available slots on " << LabTimetable::dayName(day) << ":" << endl;
        bool any = false;
        for (int lab = 1; lab <= labCount; lab++) {
            if (!labs[lab - 1].isLoaded()) continue;
            bool header = false;
            for (int p = 0; p < LabTimetable::PERIODS; p++) {
                if (!isAvailable(lab, day, p)) continue;
                if (!header) {
                    cout << "  Lab " << lab << " (" << labs[lab - 1].getRoomName() << "): ";
                    header = true;
                } else {
                    cout << ", ";
                }
                cout << LabTimetable::periodTime(p);
                any = true;
            }
            if (header) cout << endl;
        }
        if (!any) cout << "  (none)" << endl;
    }

    bool findFirstAvailable(int day, int& outLab, int& outPeriod) const {
        for (int p = 0; p < LabTimetable::PERIODS; p++) {
            for (int lab = 1; lab <= labCount; lab++) {
                if (isAvailable(lab, day, p)) {
                    outLab = lab;
                    outPeriod = p;
                    return true;
                }
            }
        }
        return false;
    }
};
#endif