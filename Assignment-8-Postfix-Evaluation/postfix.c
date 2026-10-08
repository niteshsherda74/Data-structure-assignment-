/*
 * Assignment 8: Evaluate a postfix expression using a stack in C.
 * Supports non-negative integer operands separated by spaces,
 * and operators +, -, *, /.
 * Example: 12 3 4 * + => 24
 */
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 256
#define TOKEN_SIZE 64
#define LINE_SIZE 1024

static int parse_integer(const char *token, long long *number) {
    char *end;
    long long value;
    const unsigned char *p = (const unsigned char *)token;
    if (*p == '\0') return 0;
    while (*p) {
        if (!isdigit(*p)) return 0;
        ++p;
    }
    value = strtoll(token, &end, 10);
    if (*end != '\0') return 0;
    *number = value;
    return 1;
}

static int evaluate_postfix(const char *expression, long long *result) {
    long long stack[MAX_TOKENS];
    int top = -1;
    char copy[LINE_SIZE];
    char *token;

    if (strlen(expression) >= sizeof(copy)) {
        printf("Error: expression is too long.\n");
        return 0;
    }
    strcpy(copy, expression);

    token = strtok(copy, " \t\r\n");
    while (token != NULL) {
        long long number;
        if (parse_integer(token, &number)) {
            if (top >= MAX_TOKENS - 1) {
                printf("Error: too many operands.\n");
                return 0;
            }
            stack[++top] = number;
        } else {
            long long left, right, value;
            if (strlen(token) != 1 || strchr("+-*/", token[0]) == NULL) {
                printf("Error: invalid token '%s'.\n", token);
                return 0;
            }
            if (top < 1) {
                printf("Error: not enough operands for operator '%s'.\n", token);
                return 0;
            }
            right = stack[top--];
            left = stack[top--];
            switch (token[0]) {
            case '+': value = left + right; break;
            case '-': value = left - right; break;
            case '*': value = left * right; break;
            case '/':
                if (right == 0) {
                    printf("Error: division by zero is not allowed.\n");
                    return 0;
                }
                value = left / right;
                break;
            default: return 0;
            }
            stack[++top] = value;
        }
        token = strtok(NULL, " \t\r\n");
    }

    if (top == -1) {
        printf("Error: expression is empty or contains no operands.\n");
        return 0;
    }
    if (top != 0) {
        printf("Error: invalid postfix expression; extra operands remain.\n");
        return 0;
    }
    *result = stack[top];
    return 1;
}

int main(void) {
    char expression[LINE_SIZE];
    long long result;
    printf("Enter a space-separated postfix expression: ");
    if (fgets(expression, sizeof(expression), stdin) == NULL) {
        printf("Error: unable to read expression.\n");
        return 1;
    }
    if (evaluate_postfix(expression, &result)) {
        printf("Result: %lld\n", result);
        return 0;
    }
    return 1;
}
