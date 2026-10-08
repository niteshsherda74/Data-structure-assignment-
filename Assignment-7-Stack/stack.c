/*
 * Assignment 7: Stack implementation using an array in C.
 * Operations: push, pop, peek, display, isEmpty, and isFull.
 */
#include <stdio.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
} Stack;

void initialize(Stack *stack) { stack->top = -1; }
int is_empty(const Stack *stack) { return stack->top == -1; }
int is_full(const Stack *stack) { return stack->top == MAX_SIZE - 1; }

void push(Stack *stack, int value) {
    if (is_full(stack)) {
        printf("Stack overflow: cannot push %d.\n", value);
        return;
    }
    stack->data[++stack->top] = value;
    printf("%d pushed onto the stack.\n", value);
}

void pop(Stack *stack) {
    if (is_empty(stack)) {
        printf("Stack underflow: stack is empty.\n");
        return;
    }
    printf("Popped element: %d\n", stack->data[stack->top--]);
}

void peek(const Stack *stack) {
    if (is_empty(stack)) {
        printf("Stack is empty.\n");
        return;
    }
    printf("Top element: %d\n", stack->data[stack->top]);
}

void display(const Stack *stack) {
    int i;
    if (is_empty(stack)) {
        printf("Stack is empty.\n");
        return;
    }
    printf("Stack (top to bottom): ");
    for (i = stack->top; i >= 0; --i) printf("%d ", stack->data[i]);
    putchar('\n');
}

int main(void) {
    Stack stack;
    int choice, value;
    initialize(&stack);

    do {
        printf("\n--- Stack Menu ---\n"
               "1. Push\n2. Pop\n3. Peek\n4. Display\n"
               "5. Check empty\n6. Check full\n0. Exit\n"
               "Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            return 1;
        }

        switch (choice) {
        case 1:
            printf("Enter value: ");
            if (scanf("%d", &value) != 1) {
                printf("Invalid value.\n");
                return 1;
            }
            push(&stack, value);
            break;
        case 2: pop(&stack); break;
        case 3: peek(&stack); break;
        case 4: display(&stack); break;
        case 5: printf("%s\n", is_empty(&stack) ? "Stack is empty." : "Stack is not empty."); break;
        case 6: printf("%s\n", is_full(&stack) ? "Stack is full." : "Stack is not full."); break;
        case 0: printf("Exiting.\n"); break;
        default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);
    return 0;
}
