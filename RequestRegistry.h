#ifndef REQUEST_REGISTRY_H
#define REQUEST_REGISTRY_H

enum RequestState {
    STATE_PRIORITY_QUEUED = 1,   
    STATE_WAITLISTED = 2,
    STATE_CONFIRMED = 3
};

struct RegistryEntry {
    int rollNo;
    int requestNo;       
    RequestState state;
    int slotKey;         
    int deadlineDate;
    int pageCount;
};

class RequestRegistry {
private:
    RegistryEntry* entries;
    int capacity;
    int count;

    int indexOf(int rollNo) const {
        for (int i = 0; i < count; i++)
            if (entries[i].rollNo == rollNo) return i;
        return -1;
    }

public:
    RequestRegistry(int cap = 200) {
        capacity = cap;
        count = 0;
        entries = new RegistryEntry[capacity]();
    }

    ~RequestRegistry() { delete[] entries; }

    RequestRegistry(const RequestRegistry&) = delete;
    RequestRegistry& operator=(const RequestRegistry&) = delete;

    bool isFull() const { return count == capacity; }
    int size() const { return count; }

    bool add(int rollNo, int requestNo, RequestState state, int deadlineDate, int pageCount) {
        if (count == capacity || indexOf(rollNo) != -1) return false;
        entries[count].rollNo = rollNo;
        entries[count].requestNo = requestNo;
        entries[count].state = state;
        entries[count].slotKey = -1;
        entries[count].deadlineDate = deadlineDate;
        entries[count].pageCount = pageCount;
        count++;
        return true;
    }

    bool remove(int rollNo) {
        int idx = indexOf(rollNo);
        if (idx == -1) return false;
        entries[idx] = entries[count - 1];
        count--;
        return true;
    }

    RegistryEntry* find(int rollNo) {
        int idx = indexOf(rollNo);
        return idx == -1 ? nullptr : &entries[idx];
    }

    const RegistryEntry* find(int rollNo) const {
        int idx = indexOf(rollNo);
        return idx == -1 ? nullptr : &entries[idx];
    }

    int countState(RequestState state) const {
        int c = 0;
        for (int i = 0; i < count; i++)
            if (entries[i].state == state) c++;
        return c;
    }

    const RegistryEntry& getAt(int index) const { return entries[index]; }
};

#endif
