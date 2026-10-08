/*
 * Assignment 7: Stack implementation using an array
 * Operations: push, pop, peek, display, isEmpty, and isFull
 */
#include <iostream>
using namespace std;

const int MAX_SIZE = 100;

class Stack {
private:
    int data[MAX_SIZE];
    int topIndex;

public:
    Stack() : topIndex(-1) {}

    bool isEmpty() const { return topIndex == -1; }
    bool isFull() const { return topIndex == MAX_SIZE - 1; }

    void push(int value) {
        if (isFull()) {
            cout << "Stack overflow: cannot push " << value << ".\n";
            return;
        }
        data[++topIndex] = value;
        cout << value << " pushed onto the stack.\n";
    }

    void pop() {
        if (isEmpty()) {
            cout << "Stack underflow: stack is empty.\n";
            return;
        }
        cout << "Popped element: " << data[topIndex--] << '\n';
    }

    void peek() const {
        if (isEmpty()) {
            cout << "Stack is empty.\n";
            return;
        }
        cout << "Top element: " << data[topIndex] << '\n';
    }

    void display() const {
        if (isEmpty()) {
            cout << "Stack is empty.\n";
            return;
        }
        cout << "Stack (top to bottom): ";
        for (int i = topIndex; i >= 0; --i) cout << data[i] << ' ';
        cout << '\n';
    }
};

int main() {
    Stack stack;
    int choice, value;

    do {
        cout << "\n--- Stack Menu ---\n"
             << "1. Push\n2. Pop\n3. Peek\n4. Display\n"
             << "5. Check empty\n6. Check full\n0. Exit\n"
             << "Enter choice: ";
        if (!(cin >> choice)) {
            cout << "Invalid input.\n";
            return 1;
        }

        switch (choice) {
        case 1:
            cout << "Enter value: ";
            if (cin >> value) stack.push(value);
            else { cout << "Invalid value.\n"; return 1; }
            break;
        case 2: stack.pop(); break;
        case 3: stack.peek(); break;
        case 4: stack.display(); break;
        case 5: cout << (stack.isEmpty() ? "Stack is empty.\n" : "Stack is not empty.\n"); break;
        case 6: cout << (stack.isFull() ? "Stack is full.\n" : "Stack is not full.\n"); break;
        case 0: cout << "Exiting.\n"; break;
        default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}
