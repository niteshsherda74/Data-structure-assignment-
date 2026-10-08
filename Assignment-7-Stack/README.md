# Assignment 7 — Stack Implementation

## Aim
Implement a stack and demonstrate its basic operations.

## Concepts
A stack is a linear data structure that follows **LIFO** (Last In, First Out). Insertion and deletion happen at the top.

## Operations
- `push(value)`: inserts an element at the top.
- `pop()`: removes and reports the top element.
- `peek()`: displays the top element without removing it.
- `display()`: prints elements from top to bottom.
- `isEmpty()` / `isFull()`: checks stack boundary conditions.

## Implementation
The stack is implemented using a fixed-size array of 100 integers. It checks overflow and underflow before modifying the stack.

## Compile and run
```bash
g++ -std=c++17 -Wall -Wextra -pedantic stack.cpp -o stack
./stack
```

On Windows, run `stack.exe` after compiling.

## Complexity
| Operation | Time |
|---|---:|
| Push | O(1) |
| Pop | O(1) |
| Peek | O(1) |
| Is empty / full | O(1) |
| Display | O(n) |

## Note
This is a console-based implementation for learning purposes.
