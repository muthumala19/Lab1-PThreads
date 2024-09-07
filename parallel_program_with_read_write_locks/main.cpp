#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

const int NUM_THREADS = 4;
const int N = 1000;
const int M = 10000;
const float M_MEMBER = 0.99f;
const float M_INSERT = 0.005f;
const float M_DELETE = 0.005f;

struct Node {
    int data;
    Node* next;
    Node(int value) : data(value), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
    pthread_rwlock_t rwlock;

public:
    LinkedList() : head(nullptr) {
        pthread_rwlock_init(&rwlock, nullptr);
    }

    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        pthread_rwlock_destroy(&rwlock);
    }

    bool Member(int value) {
        pthread_rwlock_rdlock(&rwlock);
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                pthread_rwlock_unlock(&rwlock);
                return true;
            }
            current = current->next;
        }
        pthread_rwlock_unlock(&rwlock);
        return false;
    }

    void Insert(int value) {
        pthread_rwlock_wrlock(&rwlock);
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;
        pthread_rwlock_unlock(&rwlock);
    }

    bool Delete(int value) {
        pthread_rwlock_wrlock(&rwlock);
        Node* current = head;
        Node* previous = nullptr;

        while (current != nullptr && current->data != value) {
            previous = current;
            current = current->next;
        }

        if (current == nullptr) {
            pthread_rwlock_unlock(&rwlock);
            return false;
        }

        if (previous == nullptr) {
            head = current->next;
        } else {
            previous->next = current->next;
        }

        delete current;
        pthread_rwlock_unlock(&rwlock);
        return true;
    }

    void Print() {
        pthread_rwlock_rdlock(&rwlock);
        Node* current = head;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
        pthread_rwlock_unlock(&rwlock);
    }
};

struct ThreadData {
    LinkedList* list;
    int num_operations;
    float mMember, mInsert, mDelete;
};

void* threadFunc(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    LinkedList* list = data->list;

    for (int i = 0; i < data->num_operations; i++) {
        float random = static_cast<float>(rand()) / RAND_MAX;

        if (random < data->mMember) {
            list->Member(rand() % (1 << 16));
        } else if (random < data->mMember + data->mInsert) {
            list->Insert(rand() % (1 << 16));
        } else {
            list->Delete(rand() % (1 << 16));
        }
    }
    return nullptr;
}

int main() {
    int num_runs = 10; // Number of times to run the test
    std::vector<double> times;

    for (int run = 0; run < num_runs; ++run) {
        LinkedList list;
        srand(time(nullptr) + run); // Seed with unique value for each run

        for (int i = 0; i < N; i++) {
            list.Insert(rand() % (1 << 16));
        }

        pthread_t threads[NUM_THREADS];
        ThreadData thread_data[NUM_THREADS];

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < NUM_THREADS; i++) {
            thread_data[i].list = &list;
            thread_data[i].num_operations = M / NUM_THREADS;
            thread_data[i].mMember = M_MEMBER;
            thread_data[i].mInsert = M_INSERT;
            thread_data[i].mDelete = M_DELETE;
            pthread_create(&threads[i], nullptr, threadFunc, &thread_data[i]);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], nullptr);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start; // Duration in milliseconds
        times.push_back(duration.count());
        std::cout << "Run " << run + 1 << ": " << duration.count() << " milliseconds" << std::endl;
    }

    // Calculate mean time
    double sum = 0;
    for (double time : times) {
        sum += time;
    }
    double mean = sum / num_runs;

    // Calculate standard deviation
    double variance = 0;
    for (double time : times) {
        variance += (time - mean) * (time - mean);
    }
    variance /= num_runs;
    double stdDev = std::sqrt(variance);

    std::cout << "Mean Execution Time: " << mean << " milliseconds" << std::endl;
    std::cout << "Standard Deviation: " << stdDev << " milliseconds" << std::endl;

    return 0;
}
