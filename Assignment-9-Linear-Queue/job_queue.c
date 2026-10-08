/*
 * Assignment 9: Job queue simulation using a linear array queue in C.
 * Jobs are processed in FIFO (First In, First Out) order.
 */
#include <stdio.h>
#include <string.h>

#define MAX_JOBS 5
#define JOB_NAME_SIZE 80

typedef struct {
    char jobs[MAX_JOBS][JOB_NAME_SIZE];
    int front;
    int rear;
} JobQueue;

void initialize(JobQueue *queue) {
    queue->front = -1;
    queue->rear = -1;
}

int is_empty(const JobQueue *queue) { return queue->front == -1; }
int is_full(const JobQueue *queue) { return queue->rear == MAX_JOBS - 1; }

void add_job(JobQueue *queue, const char *job) {
    if (is_full(queue)) {
        printf("Queue overflow: cannot add job.\n");
        return;
    }
    if (is_empty(queue)) queue->front = 0;
    ++queue->rear;
    snprintf(queue->jobs[queue->rear], JOB_NAME_SIZE, "%s", job);
    printf("Job added successfully: %s\n", job);
}

void process_job(JobQueue *queue) {
    if (is_empty(queue)) {
        printf("Queue underflow: no jobs to process.\n");
        return;
    }
    printf("Processing job: %s\n", queue->jobs[queue->front]);
    if (queue->front == queue->rear) {
        queue->front = queue->rear = -1;
    } else {
        ++queue->front;
    }
}

void display(const JobQueue *queue) {
    int i;
    if (is_empty(queue)) {
        printf("Queue is empty.\n");
        return;
    }
    printf("Pending jobs (FIFO order):\n");
    for (i = queue->front; i <= queue->rear; ++i)
        printf("- %s\n", queue->jobs[i]);
}

int main(void) {
    JobQueue queue;
    int choice;
    char job[JOB_NAME_SIZE];
    initialize(&queue);

    do {
        printf("\n--- Job Queue Menu ---\n"
               "1. Add job\n2. Process/delete job\n"
               "3. Display jobs\n4. Check empty\n5. Check full\n0. Exit\n"
               "Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            return 1;
        }
        switch (choice) {
        case 1:
            printf("Enter job name (one word): ");
            if (scanf("%79s", job) != 1) {
                printf("Invalid job name.\n");
                return 1;
            }
            add_job(&queue, job);
            break;
        case 2: process_job(&queue); break;
        case 3: display(&queue); break;
        case 4: printf("%s\n", is_empty(&queue) ? "Queue is empty." : "Queue is not empty."); break;
        case 5: printf("%s\n", is_full(&queue) ? "Queue is full." : "Queue is not full."); break;
        case 0: printf("Exiting.\n"); break;
        default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);
    return 0;
}
