# Assignment 9 — Job Queue Simulation Using Linear Queue

## Aim
Simulate a job queue using a linear queue.

## Theory
A queue follows **FIFO** (First In, First Out): the first job added is the first job processed. This program stores jobs in a fixed-size array.

## Operations
- Add job (enqueue)
- Process/delete job (dequeue)
- Display pending jobs
- Check whether the queue is empty or full

## Compile and run
```bash
g++ -std=c++17 -Wall -Wextra -pedantic job_queue.cpp -o job_queue
./job_queue
```

On Windows, run `job_queue.exe` after compiling.

## Capacity
The queue can hold up to 5 pending jobs. Because this is a **linear** queue rather than a circular queue, freed positions at the start are not reused until the queue becomes empty.

## Complexity
| Operation | Time |
|---|---:|
| Add job | O(1) |
| Process job | O(1) |
| Display jobs | O(n) |
| Check empty / full | O(1) |

## Test cases to try
1. Add one job and display the queue.
2. Add multiple jobs and confirm FIFO processing order.
3. Process a job from an empty queue.
4. Add jobs until the queue is full, then try to add another.
5. Process all jobs and confirm the queue becomes empty.
