#include <iostream>
#include "TimetableLoader.h"
#include "BookingController.h"
#include "ConsoleUI.h"
using namespace std;

bool parsePositive(const char* s, int& out) {
    if (s[0] == '\0') return false;
    int value = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return false;
        if (value > 9999) return false;
        value = value * 10 + (s[i] - '0');
    }
    if (value < 1) return false;
    out = value;
    return true;
}

int main(int argc, char* argv[]) {
    const int MAX_LABS = 14;
    const char* timetableFile = (argc > 1) ? argv[1] : "timetable.csv";
    const char* saveFile = (argc > 3) ? argv[3] : "bookings.csv";
    int priorityCapacity = 50;
    if (argc > 2 && !parsePositive(argv[2], priorityCapacity)) {
        cout << "Priority queue capacity must be a positive number. Using 50." << endl;
        priorityCapacity = 50;
    }

    LabTimetable* labs = new LabTimetable[MAX_LABS];
    int loaded = TimetableLoader::load(timetableFile, labs, MAX_LABS);
    if (loaded == 0) {
        cout << "No timetable data found in " << timetableFile
             << ". Keep it next to the program." << endl;
        delete[] labs;
        return 1;
    }
    cout << "Loaded timetable for " << loaded << " labs." << endl;

    {
        BookingController controller(labs, MAX_LABS, priorityCapacity);

        int restored = controller.loadFromFile(saveFile);
        cout << "Restored " << restored << " saved request(s)." << endl;
        controller.setAutoSaveFile(saveFile);
        cout << "[timetable: " << timetableFile << " | priority queue capacity: "
             << priorityCapacity << " | save file: " << saveFile << "]" << endl;

        ConsoleUI ui(&controller);
        ui.run();
    }

    delete[] labs;
    return 0;
}