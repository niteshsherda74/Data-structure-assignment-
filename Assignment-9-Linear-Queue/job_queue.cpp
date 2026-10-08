/*
 * Assignment 9: Job queue simulation using a linear array queue.
 * Jobs are processed in FIFO (First In, First Out) order.
 */
#include <iostream>
#include <string>
using namespace std;

const int MAX_JOBS = 5;

class JobQueue {
private:
    string jobs[MAX_JOBS];
    int frontIndex;
    int rearIndex;

public:
    JobQueue() : frontIndex(-1), rearIndex(-1) {}

    bool isEmpty() const { return frontIndex == -1; }
    bool isFull() const { return rearIndex == MAX_JOBS - 1; }

    void addJob(const string& job) {
        if (isFull()) {
            cout << "Queue overflow: cannot add job.\n";
            return;
        }
        if (isEmpty()) frontIndex = 0;
        jobs[++rearIndex] = job;
        cout << "Job added successfully: " << job << '\n';
    }

    void processJob() {
        if (isEmpty()) {
            cout << "Queue underflow: no jobs to process.\n";
            return;
        }
        cout << "Processing job: " << jobs[frontIndex] << '\n';
        if (frontIndex == rearIndex) {
            frontIndex = rearIndex = -1;
        } else {
            ++frontIndex;
        }
    }

    void display() const {
        if (isEmpty()) {
            cout << "Queue is empty.\n";
            return;
        }
        cout << "Pending jobs (FIFO order):\n";
        for (int i = frontIndex; i <= rearIndex; ++i)
            cout << "- " << jobs[i] << '\n';
    }
};

int main() {
    JobQueue queue;
    int choice;
    string job;

    do {
        cout << "\n--- Job Queue Menu ---\n"
             << "1. Add job\n2. Process/delete job\n"
             << "3. Display jobs\n4. Check empty\n5. Check full\n0. Exit\n"
             << "Enter choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input.\n";
            return 1;
        }

        switch (choice) {
        case 1:
            cout << "Enter a single-word job name: ";
            if (cin >> job) queue.addJob(job);
            else { cout << "Invalid job name.\n"; return 1; }
            break;
        case 2: queue.processJob(); break;
        case 3: queue.display(); break;
        case 4: cout << (queue.isEmpty() ? "Queue is empty.\n" : "Queue is not empty.\n"); break;
        case 5: cout << (queue.isFull() ? "Queue is full.\n" : "Queue is not full.\n"); break;
        case 0: cout << "Exiting.\n"; break;
        default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
