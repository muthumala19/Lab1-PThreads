#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cmath>
#include <vector>

const int N = 1000;
const int M = 10000;
const float M_MEMBER = 0.99f;
const float M_INSERT = 0.005f;
const float M_DELETE = 0.005f;
const int THRESHOLD=65536;
const int NUMBER_OF_RUNS=100;

struct Node {
    int data;
    Node* next;
};

void Insert(Node*& head, int value) {
    Node* newNode = new Node();
    newNode->data = value;
    newNode->next = nullptr;

    if (head == nullptr || head->data >= value) {
        newNode->next = head;
        head = newNode;
        return;
    }

    Node* current = head;
    while (current->next != nullptr && current->next->data < value) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
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
    std::vector<double> times;
    srand(time(0));

    for (int run = 0; run < NUMBER_OF_RUNS; ++run) {
        Node* head = nullptr;
        int n = N;
        int m = M;
        int mMember = M_MEMBER * m;
        int mInsert = M_INSERT * m;
        int mDelete = M_DELETE * m;

        for (int i = 0; i < n; i++) {
            int value = rand() % THRESHOLD;
            if (!Member(head, value)) {
                Insert(head, value);
            }
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < mInsert; i++) {
            int value = rand() % THRESHOLD;
            if (!Member(head, value)) {
                Insert(head, value);
            }
        }

        for (int i = 0; i < mMember; i++) {
            int value = rand() % THRESHOLD;
            Member(head, value);
        }
        for (int i = 0; i < mDelete; i++) {
            int value = rand() % THRESHOLD;
            if (!Member(head, value)) {
                Delete(head, value);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());
    }

    double sum = 0;
    for (double time : times) {
        sum += time;
    }
    double mean = sum / NUMBER_OF_RUNS;

    double variance = 0;
    for (double time : times) {
        variance += (time - mean) * (time - mean);
    }
    variance /= NUMBER_OF_RUNS;
    double stdDev = std::sqrt(variance);

    std::cout << "Mean Execution Time: " << mean << " milliseconds" << std::endl;
    std::cout << "Standard Deviation: " << stdDev << " milliseconds" << std::endl;

    return 0;
}
