#ifndef BOOKING_FILE_H
#define BOOKING_FILE_H

#include <iostream>
using namespace std;

struct SavedRequest {
    int rollNo;
    int requestNo;
    int state;          
    int slotKey; 
    int deadlineDate;
    int pageCount;
};

class BookingFile {
private:

    static bool readNumber(const char* s, int& pos, int& value) {
        bool negative = false;
        if (s[pos] == '-') {
            negative = true;
            pos++;
        }
        int start = pos;
        int v = 0;
        while (s[pos] >= '0' && s[pos] <= '9') {
            if (v > 99999999) return false;   
            v = v * 10 + (s[pos] - '0');
            pos++;
        }
        if (pos == start) return false;
        value = negative ? -v : v;
        return true;
    }

public:
    static void writeLine(ostream& out, const SavedRequest& s) {
        out << s.rollNo << ',' << s.requestNo << ',' << s.state << ','
            << s.slotKey << ',' << s.deadlineDate << ',' << s.pageCount << '\n';
    }

    static bool parseLine(const char* line, SavedRequest& out) {
        int values[6];
        int pos = 0;
        for (int i = 0; i < 6; i++) {
            if (!readNumber(line, pos, values[i])) return false;
            if (i < 5) {
                if (line[pos] != ',') return false;
                pos++;
            }
        }
        if (line[pos] == '\r') pos++;
        if (line[pos] != '\0') return false;

        out.rollNo = values[0];
        out.requestNo = values[1];
        out.state = values[2];
        out.slotKey = values[3];
        out.deadlineDate = values[4];
        out.pageCount = values[5];
        return true;
    }
};

#endif
