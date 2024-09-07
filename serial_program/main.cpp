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
    int numRuns = 20;

    std::vector<double> times;
    srand(time(0));

    for (int run = 0; run < numRuns; ++run) {
        Node* head = nullptr;
        int n = N;
        int m = M;
        int mMember = M_MEMBER * m;
        int mInsert = M_INSERT * m;
        int mDelete = M_DELETE * m;

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
        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());
        std::cout << "Run " << run + 1 << ": " << duration.count() << " milliseconds" << std::endl;
    }

    // Calculate mean time
    double sum = 0;
    for (double time : times) {
        sum += time;
    }
    double mean = sum / numRuns;

    // Calculate standard deviation
    double variance = 0;
    for (double time : times) {
        variance += (time - mean) * (time - mean);
    }
    variance /= numRuns;
    double stdDev = std::sqrt(variance);

    std::cout << "Mean Execution Time: " << mean << " milliseconds" << std::endl;
    std::cout << "Standard Deviation: " << stdDev << " milliseconds" << std::endl;

    return 0;
}
