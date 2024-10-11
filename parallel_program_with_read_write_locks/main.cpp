#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cmath>
#include <vector>
#include <pthread.h>

const int N = 1000;
const int M = 10000;
const float M_MEMBER = 0.99f;
const float M_INSERT = 0.005f;
const float M_DELETE = 0.005f;
const int THRESHOLD = 65536;
const int NUMBER_OF_THREADS = 8;
const int NUMBER_OF_RUNS=100;

struct Node {
    int data;
    Node* next;
};

struct ThreadData {
    int thread_index;
};

Node* head = nullptr;
pthread_rwlock_t list_rwlock;

void Insert(Node*& head, int value) {
    pthread_rwlock_wrlock(&list_rwlock); // Acquire write lock

    Node* newNode = new Node();
    newNode->data = value;
    newNode->next = nullptr;

    if (head == nullptr || head->data >= value) {
        newNode->next = head;
        head = newNode;
    } else {
        Node* current = head;
        while (current->next != nullptr && current->next->data < value) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    pthread_rwlock_unlock(&list_rwlock); // Release write lock
}

void Delete(Node*& head, int value) {
    pthread_rwlock_wrlock(&list_rwlock); // Acquire write lock

    Node* temp = head;
    Node* prev = nullptr;

    if (temp != nullptr && temp->data == value) {
        head = temp->next;
        delete temp;
        pthread_rwlock_unlock(&list_rwlock);
        return;
    }

    while (temp != nullptr && temp->data != value) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr) {
        pthread_rwlock_unlock(&list_rwlock);
        return;
    }

    prev->next = temp->next;
    delete temp;

    pthread_rwlock_unlock(&list_rwlock);  // Release write lock
}

bool Member(Node* head, int value) {
    pthread_rwlock_rdlock(&list_rwlock); // Acquire read lock

    Node* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            pthread_rwlock_unlock(&list_rwlock);
            return true;
        }
        temp = temp->next;
    }

    pthread_rwlock_unlock(&list_rwlock);  // Release read lock
    return false;
}

void* ThreadWork(void* arg) {
    int workLoadOnThreads = M / NUMBER_OF_THREADS;
    int mMember = M_MEMBER * workLoadOnThreads;
    int mInsert = M_INSERT * workLoadOnThreads;
    int mDelete = M_DELETE * workLoadOnThreads;

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

    return nullptr;
}

int main() {
    std::vector<double> times;
    srand(time(0));

    for (int run = 0; run < NUMBER_OF_RUNS; ++run) {
        head = nullptr;
        pthread_rwlock_init(&list_rwlock, nullptr);

        for (int i = 0; i < N; i++) {
            int value = rand() % THRESHOLD;
            if (!Member(head, value)) {
                Insert(head, value);
            }
        }

        pthread_t threads[NUMBER_OF_THREADS];
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            pthread_create(&threads[i], nullptr, ThreadWork, nullptr);
        }

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            pthread_join(threads[i], nullptr);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());

        pthread_rwlock_destroy(&list_rwlock);
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Calculate mean time
    double sum = 0;
    for (double time : times) {
        sum += time;
    }
    double mean = sum / NUMBER_OF_RUNS;

    // Calculate standard deviation
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
