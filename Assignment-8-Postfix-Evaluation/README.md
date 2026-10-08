# Assignment 8 — Postfix Expression Evaluation in C

## Aim
Evaluate a postfix expression using a stack in the C language.

## Theory
In postfix notation, an operator appears after its operands. For example, `3 4 +` represents `3 + 4`. A stack stores operands until an operator is encountered.

## Algorithm
1. Read tokens from left to right.
2. If a token is an integer, push it onto the stack.
3. If a token is an operator, pop the right operand and then the left operand.
4. Apply the operator and push the result.
5. A valid expression must leave exactly one value on the stack.

## Supported input
Space-separated, non-negative integer operands and operators `+`, `-`, `*`, and `/`. Division uses integer division.

Example:
```text
Input:  12 3 4 * +
Output: 24
```

## Compile and run
```bash
gcc -std=c11 -Wall -Wextra -pedantic postfix.c -o postfix
./postfix
```
On Windows, run `postfix.exe` after compiling.

## Complexity
For `n` tokens, time complexity is **O(n)** and auxiliary space complexity is **O(n)**.

The program checks invalid tokens, insufficient operands, extra operands, and division by zero.
