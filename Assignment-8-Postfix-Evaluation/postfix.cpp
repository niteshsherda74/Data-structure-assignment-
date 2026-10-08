/*
 * Assignment 8: Evaluate a postfix expression using a stack.
 * Supports non-negative integer operands separated by spaces,
 * and operators +, -, *, /.
 * Example: 12 3 4 * +  => 24
 */
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
using namespace std;

bool isIntegerToken(const string& token) {
    if (token.empty()) return false;
    for (char ch : token) {
        if (ch < '0' || ch > '9') return false;
    }
    return true;
}

bool evaluatePostfix(const string& expression, long long& result, string& error) {
    stack<long long> values;
    istringstream input(expression);
    string token;

    while (input >> token) {
        if (isIntegerToken(token)) {
            try {
                size_t used = 0;
                long long number = stoll(token, &used);
                if (used != token.size()) {
                    error = "Invalid number: " + token;
                    return false;
                }
                values.push(number);
            } catch (...) {
                error = "Number is outside the supported range: " + token;
                return false;
            }
            continue;
        }

        if (token.size() != 1 || string("+-*/").find(token[0]) == string::npos) {
            error = "Invalid token: " + token;
            return false;
        }
        if (values.size() < 2) {
            error = "Not enough operands for operator: " + token;
            return false;
        }

        long long right = values.top(); values.pop();
        long long left = values.top(); values.pop();
        long long value = 0;

        switch (token[0]) {
        case '+': value = left + right; break;
        case '-': value = left - right; break;
        case '*': value = left * right; break;
        case '/':
            if (right == 0) {
                error = "Division by zero is not allowed.";
                return false;
            }
            value = left / right;
            break;
        }
        values.push(value);
    }

    if (values.empty()) {
        error = "Expression is empty or contains no operands.";
        return false;
    }
    if (values.size() != 1) {
        error = "Invalid postfix expression: extra operands remain.";
        return false;
    }

    result = values.top();
    return true;
}

int main() {
    string expression;
    cout << "Enter a space-separated postfix expression: ";
    getline(cin, expression);

    long long result = 0;
    string error;
    if (evaluatePostfix(expression, result, error)) {
        cout << "Result: " << result << '\n';
    } else {
        cout << "Error: " << error << '\n';
        return 1;
    }
    return 0;
}
