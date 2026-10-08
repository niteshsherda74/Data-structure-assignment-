# Assignment 7 — Stack Implementation in C

## Aim
Implement a stack and demonstrate its basic operations using the C language.

## Theory
A stack is a linear data structure that follows **LIFO** (Last In, First Out). Insertion and deletion happen at the top.

## Operations
- `push(value)`: inserts an element at the top.
- `pop()`: removes and reports the top element.
- `peek()`: displays the top element without removing it.
- `display()`: prints elements from top to bottom.
- `is_empty()` / `is_full()`: checks stack boundary conditions.

## Compile and run
```bash
gcc -std=c11 -Wall -Wextra -pedantic stack.c -o stack
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

The implementation uses a fixed-size array of 100 integers and checks overflow and underflow.
