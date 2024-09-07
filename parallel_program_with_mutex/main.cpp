#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>
#include <cmath>

struct Node {
    int data;
    Node* next;
};

pthread_mutex_t list_mutex;
pthread_mutex_t print_mutex;

void Insert(Node*& head, int value) {
    pthread_mutex_lock(&list_mutex);
    Node* newNode = new Node();
    newNode->data = value;
    newNode->next = head;
    head = newNode;
    pthread_mutex_unlock(&list_mutex);
}

void Delete(Node*& head, int value) {
    pthread_mutex_lock(&list_mutex);
    Node* temp = head;
    Node* prev = nullptr;

    if (temp != nullptr && temp->data == value) {
        head = temp->next;
        delete temp;
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    while (temp != nullptr && temp->data != value) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr) {
        pthread_mutex_unlock(&list_mutex);
        return;
    }

    prev->next = temp->next;
    delete temp;
    pthread_mutex_unlock(&list_mutex);
}

bool Member(Node* head, int value) {
    pthread_mutex_lock(&list_mutex);
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            pthread_mutex_unlock(&list_mutex);
            return true;
        }
        temp = temp->next;
    }
    pthread_mutex_unlock(&list_mutex);
    return false;
}

void* ThreadFunc(void* arg) {
    Node* head = static_cast<Node*>(arg);
    int m = 10000;
    int mMember = 0.99 * m;
    int mInsert = 0.005 * m;
    int mDelete = 0.005 * m;

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
    return nullptr;
}

int main() {
    int n = 1000;
    int num_threads = 4;
    int num_runs = 10; // Number of times to run the test
    std::vector<double> times;
    pthread_t threads[num_threads];

    srand(time(0));

    pthread_mutex_init(&list_mutex, nullptr);
    pthread_mutex_init(&print_mutex, nullptr);

    for (int run = 0; run < num_runs; ++run) {
        Node* head = nullptr;

        for (int i = 0; i < n; i++) {
            int value = rand() % 65536;
            Insert(head, value);
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], nullptr, ThreadFunc, head);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], nullptr);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start; // Duration in milliseconds
        times.push_back(duration.count());
        std::cout << "Run " << run + 1 << ": " << duration.count() << " milliseconds" << std::endl;

        // Clean up the list
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
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

    pthread_mutex_destroy(&list_mutex);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}
