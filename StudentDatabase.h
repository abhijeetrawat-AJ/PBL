#ifndef STUDENT_DATABASE_H
#define STUDENT_DATABASE_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct StudentRecord {
    string id;
    string pin;
    string name;
    int year;
    bool isValid;

    StudentRecord() : id(""), pin(""), name(""), year(0), isValid(false) {}
    StudentRecord(string i, string p, string n, int y) : id(i), pin(p), name(n), year(y), isValid(true) {}
};

class StudentDatabase {
private:
    static const int MAX_STUDENTS = 100;
    StudentRecord records[MAX_STUDENTS];
    int count;

public:
    StudentDatabase() : count(0) {}

    void loadStudents(const string& filename = "students.csv") {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[System Info] Generating default Student Database..." << endl;
            addStudent("23011711", "1234", "Abhijeet Rawat", 2);
            addStudent("23011712", "0000", "Aayush Rana", 2);
            addStudent("23011713", "1111", "Prince", 2);
            addStudent("1001", "9999", "Test Senior", 4);
            addStudent("1002", "8888", "Test Junior", 1);
            saveStudents(filename);
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string id, pin, name, yearStr;
            if (getline(ss, id, ',') && getline(ss, pin, ',') && 
                getline(ss, name, ',') && getline(ss, yearStr, ',')) {
                addStudent(id, pin, name, stoi(yearStr));
            }
        }
        file.close();
        cout << "[System] Student Database loaded." << endl;
    }

    void saveStudents(const string& filename = "students.csv") {
        ofstream file(filename);
        if (!file.is_open()) return;
        for (int i = 0; i < count; i++) {
            if (records[i].isValid) {
                file << records[i].id << "," << records[i].pin << "," 
                     << records[i].name << "," << records[i].year << "\n";
            }
        }
        file.close();
    }

    bool addStudent(string id, string pin, string name, int year) {
        if (count >= MAX_STUDENTS) return false;
        records[count++] = StudentRecord(id, pin, name, year);
        return true;
    }

    bool authenticate(string id, string pin, int& outYear, string& outName) {
        for (int i = 0; i < count; i++) {
            if (records[i].id == id && records[i].isValid) {
                if (records[i].pin == pin) {
                    outYear = records[i].year;
                    outName = records[i].name;
                    return true;
                } else {
                    cout << "[Auth Error] Invalid PIN provided for Student ID " << id << endl;
                    return false;
                }
            }
        }
        cout << "[Auth Error] Student ID not found in the college database." << endl;
        return false;
    }
};

#endif