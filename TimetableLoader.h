#ifndef TIMETABLE_LOADER_H
#define TIMETABLE_LOADER_H

#include <fstream>
#include <iostream>
#include "LabTimetable.h"
using namespace std;

class TimetableLoader {
private:
    static int readInt(const char* line, int& pos) {
        int value = 0;
        bool found = false;
        while (line[pos] >= '0' && line[pos] <= '9') {
            value = value * 10 + (line[pos] - '0');
            pos++;
            found = true;
        }
        if (found) {
            return value;
        }
        else {
            return -1;
        }
    }

    static bool skipComma(const char* line, int& pos) {
        if (line[pos] == ',') {
            pos++;
            return true;
        }
        return false;
    }

    static void warn(int lineNo) {
        cout << "[Warning] timetable.csv line " << lineNo << " skipped (bad format)" << endl;
    }

public:
    static int load(const char* filename, LabTimetable* labs, int maxLabs,
                    bool pblBlocksSlot = true) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[Error] Cannot open " << filename << endl;
            return 0;
        }

        char line[128];
        int lineNo = 0;
        while (file.getline(line, 128)) {
            lineNo++;
            if (line[0] == '\0' || line[0] == '#' || line[0] == '\r') continue;

            int pos = 0;
            int labNo = readInt(line, pos);
            if (labNo < 1 || labNo > maxLabs || !skipComma(line, pos)) {
                warn(lineNo);
                continue;
            }

            char room[16];
            int r = 0;
            while (line[pos] != ',' && line[pos] != '\0' && r < 15) {
                room[r++] = line[pos++];
            }
            room[r] = '\0';
            if (!skipComma(line, pos)) {
                warn(lineNo);
                continue;
            }

            int day = readInt(line, pos);
            if (day < 0 || day >= LabTimetable::DAYS || !skipComma(line, pos)) {
                warn(lineNo);
                continue;
            }

            bool ok = true;
            for (int p = 0; p < LabTimetable::PERIODS; p++) {
                char c = line[pos + p];
                if (c != '0' && c != '1' && c != '2') {
                    ok = false;
                    break;
                }
            }
            if (!ok) {
                warn(lineNo);
                continue;
            }

            LabTimetable& lab = labs[labNo - 1];
            if (!lab.isLoaded()) lab.init(labNo, room);

            for (int p = 0; p < LabTimetable::PERIODS; p++) {
                char c = line[pos + p];
                bool occ = (c != '0');
                if (c == '2' && !pblBlocksSlot) occ = false;
                lab.setOccupied(day, p, occ);
            }
        }
        file.close();

        int loaded = 0;
        for (int i = 0; i < maxLabs; i++)
            if (labs[i].isLoaded()) loaded++;
        return loaded;
    }
};

#endif
