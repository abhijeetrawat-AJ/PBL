#ifndef INPUT_HELPER_H
#define INPUT_HELPER_H

#include <iostream>
using namespace std;
class InputHelper {
private:
    static bool parseInt(const char* s, int& value) {
        int i = 0;
        while (s[i] == ' ' || s[i] == '\t') i++;
        int start = i;
        int v = 0;
        while (s[i] >= '0' && s[i] <= '9') {
            if (v > 99999999) return false;
            v = v * 10 + (s[i] - '0');
            i++;
        }
        if (i == start) return false;
        while (s[i] == ' ' || s[i] == '\t' || s[i] == '\r') i++;
        if (s[i] != '\0') return false;
        value = v;
        return true;
    }

public:
    static bool readInt(const char* prompt, int minVal, int maxVal, int& out) {
        char buf[64];
        while (true) {
            cout << prompt;
            if (!cin.getline(buf, 64)) {
                if (cin.eof()) return false;
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Input too long. Try again." << endl;
                continue;
            }
            int value = 0;
            if (parseInt(buf, value) && value >= minVal && value <= maxVal) {
                out = value;
                return true;
            }
            cout << "Invalid input. Enter a number between "
                 << minVal << " and " << maxVal << "." << endl;
        }
    }
};

#endif
