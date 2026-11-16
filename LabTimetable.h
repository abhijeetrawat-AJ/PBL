#ifndef LAB_TIMETABLE_H
#define LAB_TIMETABLE_H

#include <iostream>
using namespace std;

class LabTimetable {
public:
    static const int DAYS = 6;      
    static const int PERIODS = 10;  

private:
    int labNo;
    char roomName[16];               
    bool occupied[DAYS][PERIODS];    

    static bool inRange(int day, int period) {
        return day >= 0 && day < DAYS && period >= 0 && period < PERIODS;
    }

public:
    LabTimetable() {
        labNo = 0;
        roomName[0] = '\0';
        for (int d = 0; d < DAYS; d++)
            for (int p = 0; p < PERIODS; p++)
                occupied[d][p] = true;
    }

    void init(int no, const char* room) {
        labNo = no;
        int i = 0;
        while (room[i] != '\0' && i < 15) {
            roomName[i] = room[i];
            i++;
        }
        roomName[i] = '\0';
    }

    bool isLoaded() const { return labNo != 0; }
    int getLabNo() const { return labNo; }
    const char* getRoomName() const { return roomName; }

    bool isOccupied(int day, int period) const {
        if (!inRange(day, period)) return true;
        return occupied[day][period];
    }

    bool isFree(int day, int period) const {
        return !isOccupied(day, period);
    }

    bool setOccupied(int day, int period, bool value) {
        if (!inRange(day, period)) return false;
        occupied[day][period] = value;
        return true;
    }

    int countFreeSlots() const {
        int count = 0;
        for (int d = 0; d < DAYS; d++)
            for (int p = 0; p < PERIODS; p++)
                if (!occupied[d][p]) count++;
        return count;
    }

    static int makeSlotKey(int lab, int day, int period) {
        return lab * 100 + day * 10 + period;
    }

    static const char* dayName(int day) {
        static const char* names[DAYS] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        if (day < 0 || day >= DAYS) return "???";
        return names[day];
    }

    static const char* periodTime(int period) {
        static const char* times[PERIODS] = {
            "08:00-08:55", "08:55-09:50", "10:10-11:05", "11:05-12:00",
            "12:00-12:55", "12:55-13:50", "14:10-15:05", "15:05-16:00",
            "16:00-16:55", "16:55-17:50"};
        if (period < 0 || period >= PERIODS) return "???";
        return times[period];
    }

    void print() const {
        cout << "Lab " << labNo << " (Room " << roomName << ")" << endl;
        cout << "     ";
        for (int p = 0; p < PERIODS; p++) cout << p << ' ';
        cout << endl;
        for (int d = 0; d < DAYS; d++) {
            cout << dayName(d) << "  ";
            for (int p = 0; p < PERIODS; p++)
                cout << (occupied[d][p] ? '#' : '.') << ' ';
            cout << endl;
        }
    }
};

#endif