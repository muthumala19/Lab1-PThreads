#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

const int N = 1000;
const int M = 10000;
const float M_MEMBER = 0.99f;
const float M_INSERT = 0.005f;
const float M_DELETE = 0.005f;

// Node structure for linked list
struct Node {
    int data;
    Node* next;
};

void Insert(Node*& head, int value) {
    Node* newNode = new Node();
    newNode->data = value;
    newNode->next = head;
    head = newNode;
}

void Delete(Node*& head, int value) {
    Node* temp = head;
    Node* prev = nullptr;

    if (temp != nullptr && temp->data == value) {
        head = temp->next;
        delete temp;
        return;
    }

    while (temp != nullptr && temp->data != value) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr) return;

    prev->next = temp->next;
    delete temp;
}

bool Member(Node* head, int value) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->data == value)
            return true;
        temp = temp->next;
    }
    return false;
}

int main() {
    Node* head = nullptr;
    int n = N;
    int m = M;
    int mMember = M_MEMBER * m;
    int mInsert = M_INSERT * m;
    int mDelete = M_DELETE * m;

    srand(time(0));

    // Populate the list with n random unique values
    for (int i = 0; i < n; i++) {
        int value = rand() % 65536;
        Insert(head, value);
    }

    auto start = std::chrono::high_resolution_clock::now();

    // Perform m operations (Member, Insert, Delete)
    for (int i = 0; i < m; i++) {
        int operation = rand() % 3;
        int value = rand() % 65536;

        if (operation == 0 && mMember > 0) {
            Member(head, value);
            mMember--;
        } else if (operation == 1 && mInsert > 0) {
            if (!Member(head, value)) {
                Insert(head, value);
            }
            mInsert--;
        } else if (operation == 2 && mDelete > 0) {
            Delete(head, value);
            mDelete--;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution Time: " << duration.count() << " seconds" << std::endl;

    return 0;
}
