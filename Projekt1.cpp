#include <iostream>
#include <cstring>
#include "DataStructs.cpp"
#define SIZE 5
using namespace std;

int priority(char operation) {
    switch (operation) {
    case '+':
    case '-': return 1;
    case '*':
    case '/': return 2;
    case 'F': return 3; // funkcja IF
    case 'N': return 4; // funkcja N
    case 'I':  // funkcja MIN
    case 'A': return 5; // funkcja MAX
    default: return 0;
    }
}

int executeOperation(char operation, int op1, int op2, bool& error) {
    switch (operation)
    {
    case '+': return op1 + op2;
    case '-': return op1 - op2;
    case '*': return op1 * op2;
    case '/':
        if (op2 == 0) {
            error = true; // flaga dla dzielenia przez 0
            return 0;
        }
        else {
            return op1 / op2;
        }
    default:
        return 0;
    }
}

bool isNumber(char* token) {
    for (int i = 0; token[i] != '\0'; i++) {
        if (!isdigit(token[i])) {
            return false;
        }
    }
    return true;
}

bool isOperator(char* token) {
    return token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/';
}

bool isMAX_MIN(char* token) {
    return token[1] == 'A' || token[1] == 'I'; // sprawdzenie czy token to MAX lub MIN 
}

void postfixNotation(Queue<char*>& outputQueue, Queue<char*>& tempQueue) { // wypisanie wyrazenia w notacji postfix
    while (!outputQueue.isEmpty()) {
        char* item = outputQueue.dequeue();
        cout << item << " ";
        tempQueue.enqueue(item);
    }
    cout << endl;
}

void conversionRNP(Queue<char*>& outputQueue) {
    Stack<char*> operatorStack;
    Stack<int> countArguments; // licznik argumentow dla MIN i MAX
    Queue<char*> tempQueue; // tymczasowa kolejka
    Stack<bool> isIF; // flaga czy jest to IF 
    char tokenBuffer[SIZE];

    while (cin >> tokenBuffer && tokenBuffer[0] != '.') {
        char* token = new char[SIZE];
        strcpy(token, tokenBuffer);

        if (isNumber(token)) {
            outputQueue.enqueue(token);
        }
        else if (token[0] == 'N') { // sprawdzenie czy token to funkcja N
            operatorStack.push(token);
        }
        else if (token[1] == 'F') { // sprawdzenie czy token to funkcja IF
            operatorStack.push(token);
            isIF.push(true);
        }
        else if (isMAX_MIN(token)) {
            countArguments.push(0);
            operatorStack.push(token);
            isIF.push(false);
        }
        else if (isOperator(token)) {
            while (!operatorStack.isEmpty() && operatorStack.peek()[0] != '(' && priority(operatorStack.peek()[0]) >= priority(token[0])) {
                outputQueue.enqueue(operatorStack.pop());
            }
            operatorStack.push(token);
        }
        else if (token[0] == ',') { // czy token to ","
            if (!countArguments.isEmpty() && !isIF.peek()) { // sprawdz czy nie jestes w funkcji IF
                int currentNumber = countArguments.pop() + 1;
                countArguments.push(currentNumber); // zwieksza ilosc argumnetow o 1 
            }
            while (operatorStack.peek()[0] != '(') {
                outputQueue.enqueue(operatorStack.pop());
            }
        }
        else if (token[0] == '(') {
            operatorStack.push(token);
        }
        else if (token[0] == ')') {
            while (!operatorStack.isEmpty() && operatorStack.peek()[0] != '(') {
                outputQueue.enqueue(operatorStack.pop());
            }
            if (!operatorStack.isEmpty() && operatorStack.peek()[0] == '(') {
                operatorStack.pop();
            }
            if (!operatorStack.isEmpty() && operatorStack.peek()[1] == 'F') {
                outputQueue.enqueue(operatorStack.pop());
                if (!isIF.isEmpty()) {
                    isIF.pop();
                }
            }
            if (!operatorStack.isEmpty() && isMAX_MIN(operatorStack.peek())) {
                int arguments = (!countArguments.isEmpty()) ? countArguments.pop() + 1 : 1; // liczba argumentow = liczba przecinkow + 1 lub 1 argument, gdy nie ma przecinka. 
                char* functionToken = operatorStack.pop();
                char* newToken = new char[SIZE];
                sprintf(newToken, "%s%d", functionToken, arguments); // token w formacie MAXi lub MINi
                outputQueue.enqueue(newToken);
                delete[] functionToken;
                if (!isIF.isEmpty()) {
                    isIF.pop();
                }
            }
            delete[] token;
        }
        else {
            delete[] token;
        }
    }

    while (!operatorStack.isEmpty()) {
        if (operatorStack.peek()[0] != '(') {
            char* item = operatorStack.pop();
            outputQueue.enqueue(item);
        }
    }

    postfixNotation(outputQueue, tempQueue);

    while (!tempQueue.isEmpty()) {
        outputQueue.enqueue(tempQueue.dequeue());
    }
}

void calculateRPN(Queue<char*>& outputQueue) {
    Stack<int> operandStack;
    bool error = false;

    while (!outputQueue.isEmpty()) {
        char* token = outputQueue.dequeue();
        if (isNumber(token)) {
            int number = atoi(token);
            operandStack.push(number);
        }
        else {
            if (!operandStack.isEmpty()) {
                cout << token << " ";
                operandStack.printStack();
                cout << endl;

                if (token[0] == 'N') {
                    int op1 = operandStack.pop();
                    int result = -op1;
                    operandStack.push(result);
                }
                else if (token[1] == 'F') {
                    int op3 = operandStack.pop();
                    int op2 = operandStack.pop();
                    int op1 = operandStack.pop();
                    int result = (op1 > 0) ? op2 : op3;
                    operandStack.push(result);
                }
                else if (token[1] == 'A') {
                    int arguments = atoi(token + 3); // liczba argumentow z MAXi 
                    int max = operandStack.pop(); // pierwszy argument funkcji 
                    for (int i = 1; i < arguments; i++) {
                        int op = operandStack.pop();
                        if (max < op) {
                            max = op;
                        }
                    }
                    operandStack.push(max);
                }
                else if (token[1] == 'I') {
                    int arguments = atoi(token + 3); // liczba argumentow z MINi
                    int min = operandStack.pop();
                    for (int i = 1; i < arguments; i++) {
                        int op = operandStack.pop();
                        if (op < min) {
                            min = op;
                        }
                    }
                    operandStack.push(min);
                }
                else {
                    int op2 = operandStack.pop();
                    int op1 = operandStack.pop();

                    int result = executeOperation(token[0], op1, op2, error);
                    if (error) { // jezeli dzielenie przez zero
                        cout << "ERROR" << endl;
                        delete[] token;
                        outputQueue.~Queue(); // czyszczenie kolejki
                        cout << endl;
                        error = false;
                        return;
                    }
                    operandStack.push(result);
                }
            }
        }
        delete[] token;
    }
    int result = operandStack.pop();
    cout << result << endl;
    cout << endl;
}

int main() {
    Queue<char*> outputQueue;

    int n;
    cin >> n;

    for (int i = 0; i < n; i++) {
        conversionRNP(outputQueue);
        calculateRPN(outputQueue);
    }

    return 0;
}