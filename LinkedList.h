#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct HashNode {
    int key;        // Slot key
    bool isFree;    // Indicates whether the slot is free or occupied
    HashNode* next;

    HashNode(int k, bool f) {
        key = k;
        isFree = f;
        next = nullptr;
    }
};

class LinkedList {
private:
    HashNode* head;

public:
    LinkedList() { head = nullptr; }

    ~LinkedList() {
        HashNode* curr = head;
        while (curr != nullptr) {
            HashNode* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    // Rule of Three: Copying is disabled to prevent double deletion
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // Returns false if the key already exists
    bool insert(int key, bool isFree) {
        if (contains(key)) return false;
        HashNode* newNode = new HashNode(key, isFree);
        newNode->next = head;
        head = newNode;
        return true;
    }

    bool contains(int key) const {
        HashNode* curr = head;
        while (curr != nullptr) {
            if (curr->key == key) return true;
            curr = curr->next;
        }
        return false;
    }

    bool search(int key, bool& status) const {
        HashNode* curr = head;
        while (curr != nullptr) {
            if (curr->key == key) {
                status = curr->isFree;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool updateStatus(int key, bool isFree) {
        HashNode* curr = head;
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->isFree = isFree;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool remove(int key) {
        HashNode* curr = head;
        HashNode* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) head = curr->next;
                else prev->next = curr->next;

                delete curr;
                return true;
            }

            prev = curr;
            curr = curr->next;
        }

        return false;
    }

    HashNode* getHead() const { return head; }
};

#endif
