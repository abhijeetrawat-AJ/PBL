#ifndef OWNER_AUTH_H
#define OWNER_AUTH_H

#include <string>
#include <iostream>

using namespace std;

class OwnerAuth {
private:
    string ownerId;
    string ownerPin;

public:
    OwnerAuth() : ownerId("admin"), ownerPin("admin123") {}

    bool authenticate(const string& id, const string& pin) {
        if (id == ownerId && pin == ownerPin) {
            return true;
        }
        cout << "[Auth Error] Invalid Owner ID or PIN." << endl;
        return false;
    }
};

#endif