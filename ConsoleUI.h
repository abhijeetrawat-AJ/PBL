#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <iostream>
#include "BookingController.h"
#include "InputHelper.h"
using namespace std;
class ConsoleUI {
private:
    BookingController* controller;   

    static const int MAX_ROLL = 99999999;
    static const int MAX_PAGES = 99999;

    static const char* submitMessage(SubmitResult r) {
        switch (r) {
            case SUBMIT_BOOKED: return "Slot booked!";
            case SUBMIT_QUEUED_PRIORITY: return "No free slot right now. Added to the priority queue.";
            case SUBMIT_WAITLISTED: return "Priority queue is full. Added to the waitlist.";
            case SUBMIT_DUPLICATE: return "You already have an active request. Cancel it first.";
            case SUBMIT_INVALID_ROLL: return "Invalid roll number.";
            case SUBMIT_INVALID_DATE: return "Invalid deadline date (use a real date, YYYYMMDD).";
            case SUBMIT_INVALID_PAGES: return "Too many pages for one 55-minute slot (max 265). Split the print job.";
            case SUBMIT_SYSTEM_FULL: return "System is full. Try again later.";
        }
        return "Unknown result.";
    }

    // false = input khatam (EOF)
    bool actionShowAvailable() const {
        int day;
        if (!InputHelper::readInt("Day (1=Mon ... 6=Sat, 0=all days): ", 0, 6, day)) return false;
        if (day == 0) {
            for (int d = 0; d < LabTimetable::DAYS; d++) controller->printAvailableOnDay(d);
        } else {
            controller->printAvailableOnDay(day - 1);
        }
        return true;
    }

    bool actionRequest() {
        int roll, deadline, pages;
        if (!InputHelper::readInt("Roll number: ", 1, MAX_ROLL, roll)) return false;
        if (controller->hasActiveRequest(roll)) {   // pehle hi rok do, baaki input mat maango
            cout << ">>> " << submitMessage(SUBMIT_DUPLICATE) << endl;
            return true;
        }
        if (!InputHelper::readInt("DNP deadline (YYYYMMDD, e.g. 20261120): ", 0, MAX_ROLL, deadline)) return false;
        if (!InputHelper::readInt("Total pages to print: ", 1, MAX_PAGES, pages)) return false;

        SubmitResult r = controller->submitRequest(roll, deadline, pages);
        cout << ">>> " << submitMessage(r) << endl;
        if (r == SUBMIT_BOOKED || r == SUBMIT_QUEUED_PRIORITY || r == SUBMIT_WAITLISTED) {
            controller->printStatus(roll);
        }
        return true;
    }

    bool actionStatus() const {
        int roll;
        if (!InputHelper::readInt("Roll number: ", 1, MAX_ROLL, roll)) return false;
        controller->printStatus(roll);
        return true;
    }

    bool actionCancel() {
        int roll;
        if (!InputHelper::readInt("Roll number: ", 1, MAX_ROLL, roll)) return false;
        int reassigned;
        CancelResult r = controller->cancelRequest(roll, reassigned);
        if (r == CANCEL_NOT_FOUND) {
            cout << ">>> No active request for roll " << roll << "." << endl;
            return true;
        }
        cout << ">>> " << (r == CANCEL_BOOKING ? "Booking" : "Queued request") << " cancelled." << endl;
        if (reassigned != -1) {
            cout << ">>> Freed slot reassigned to roll " << reassigned << "." << endl;
        }
        return true;
    }

    void studentMenu() {
        while (true) {
            cout << "\n===== Student Portal =====" << endl;
            cout << "1. View available slots" << endl;
            cout << "2. Request a slot" << endl;
            cout << "3. View my booking status" << endl;
            cout << "4. Cancel my request" << endl;
            cout << "5. Back" << endl;
            int choice;
            if (!InputHelper::readInt("Enter choice: ", 1, 5, choice)) return;
            if (choice == 5) return;

            bool ok = true;
            if (choice == 1) ok = actionShowAvailable();
            else if (choice == 2) ok = actionRequest();
            else if (choice == 3) ok = actionStatus();
            else if (choice == 4) ok = actionCancel();
            if (!ok) return;
        }
    }

    void ownerMenu() {
        while (true) {
            cout << "\n===== Owner / Admin Panel =====" << endl;
            cout << "1. View available slots" << endl;
            cout << "2. View all requests (booked / queue / waitlist)" << endl;
            cout << "3. View a lab's timetable" << endl;
            cout << "4. Cancel a student's request" << endl;
            cout << "5. Back" << endl;
            int choice;
            if (!InputHelper::readInt("Enter choice: ", 1, 5, choice)) return;
            if (choice == 5) return;

            bool ok = true;
            if (choice == 1) {
                ok = actionShowAvailable();
            } else if (choice == 2) {
                controller->printAllRequests();
            } else if (choice == 3) {
                int lab;
                ok = InputHelper::readInt("Lab number: ", 1, controller->getLabCount(), lab);
                if (ok) controller->printLabTimetable(lab);
            } else if (choice == 4) {
                ok = actionCancel();
            }
            if (!ok) return;
        }
    }

public:
    ConsoleUI(BookingController* c) { controller = c; }

    void run() {
        while (true) {
            cout << "\n=========================================" << endl;
            cout << "   PrintQ - Lab & Printer Slot Booking   " << endl;
            cout << "=========================================" << endl;
            cout << "1. Student Portal" << endl;
            cout << "2. Owner / Admin Panel" << endl;
            cout << "3. Exit" << endl;
            int choice;
            if (!InputHelper::readInt("Enter choice: ", 1, 3, choice)) return;
            if (choice == 3) {
                cout << "Goodbye!" << endl;
                return;
            }
            if (choice == 1) studentMenu();
            else ownerMenu();
        }
    }
};

#endif
