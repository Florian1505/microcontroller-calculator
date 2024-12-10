#include <Arduino.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define BUFFER_SIZE 200 // MAximale Größe für den Eingabepuffer

char inputBuffer[BUFFER_SIZE] = {0}; // Eingabepuffer der seriellen Eingabe
int bufferIndex = 0;                 // Index für den Eingabepuffer

// Definition einer Struktur für den Stack für Zeichen
struct Stack {
    char data[BUFFER_SIZE];
    int top;
};

// Definition einer Struktur für den Stack für lange Ganzzahlen
struct StackLong {
    long data[BUFFER_SIZE];
    int top;
};

// Funktion: Initialisiert den Strack für Zeichen
void initStack(Stack* stack) {
    stack->top = -1; // Strack ist leer
}

// Funktion: Initialisiert den Strack für lange Ganzzahlen
void initStackLong(StackLong* stack) {
    stack->top = -1; // Strack ist leer
}

// Funktion: Zeichen zum Stack hinzufügen
bool push(Stack* stack, char element) {
    if (stack->top < BUFFER_SIZE - 1) {
        stack->data[++stack->top] = element;
        return true; // Erfolgreich hinzugefügt
    }
    return false; // Strack ist voll
}

// Funktion:Ganzzahl zum Stack hinzufügen
bool pushLong(StackLong* stack, long element) {
    if (stack->top < BUFFER_SIZE - 1) {
        stack->data[++stack->top] = element;
        return true; // Erfolgreich hizugefügt
    }
    return false; // Strack ist voll
}

// Funktion: Zeichen vom Stack entfernen
char pop(Stack* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return '\0'; // Strack ist leer
}

// Funktion: Ganzzahl vom Stack entfernen
long popLong(StackLong* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return 0; // Starck ist leer
}

// Funktion: Priorität von Operatoren bestimmen
int precedence(char op) {
    if (op == '+' || op == '-') return 1; // Niedrige Priorität
    if (op == '*' || op == '/') return 2; // Hohe Priorität
    return 0; // Kein Operator
}

// Funktion: Ausdruck in umgekehrte polnische Notation konvertieren (Postfix)
bool toPostfix(const char* infix, char* postfix) {
    Stack operatorStack;
    initStack(&operatorStack); // Stack für Operatoren initialisieren
    int j = 0; // Index für das Postfix-Array

    for (int i = 0; infix[i] != '\0'; i++) {
        if (isdigit(infix[i])) { // Wenn das aktuelle Zeichen eine Zahll ist
            while (isdigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' '; // Trennzeichen hinzufügen
            i--; // Index korrigieren
        } else if (infix[i] == '(') { // Wenn eine öffnende Klammer gefunden wird
            push(&operatorStack, infix[i]);
        } else if (infix[i] == ')') { // Wenn eine schließende Klammer gefunden wird
            while (operatorStack.top >= 0 && operatorStack.data[operatorStack.top] != '(') {
                postfix[j++] = pop(&operatorStack);
                postfix[j++] = ' ';
            }
            pop(&operatorStack); // Entfernt die öffnende Klammer
        } else if (strchr("+-*/", infix[i])) { // Wenn ein Operator gefunden wird
            while (operatorStack.top >= 0 && precedence(operatorStack.data[operatorStack.top]) >= precedence(infix[i])) {
                postfix[j++] = pop(&operatorStack);
                postfix[j++] = ' ';
            }
            push(&operatorStack, infix[i]); // Aktuellen Operator auf den Strack legen
        }
    }

    // Verarbeitet verbleibende Operatoren
    while (operatorStack.top >= 0) {
        postfix[j++] = pop(&operatorStack);
        postfix[j++] = ' ';
    }
    postfix[j] = '\0'; // Nullterminierung
    return true; // Erfolgreich
}

// Funktion: Ausdruck aus umgekehrter polnischer Notation berechnen
bool evaluatePostfix(const char* postfix, long* result) {
    StackLong operandStack;
    initStackLong(&operandStack); // Operandstraack initialisieren

    for (int i = 0; postfix[i] != '\0'; i++) {
        if (isdigit(postfix[i])) { // Wenn eine Zahl gefunden wird
            long num = 0;
            while (isdigit(postfix[i])) {
                num = num * 10 + (postfix[i++] - '0');
            }
            pushLong(&operandStack, num);
        } else if (strchr("+-*/", postfix[i])) { // Wenn ein Operator gefunden wird
            long b = popLong(&operandStack);
            long a = popLong(&operandStack);
            if (postfix[i] == '/' && b == 0) {
                return false; // Fehler Division durch Null
            }
            switch (postfix[i]) {
                case '+': pushLong(&operandStack, a + b); break;
                case '-': pushLong(&operandStack, a - b); break;
                case '*': pushLong(&operandStack, a * b); break;
                case '/': pushLong(&operandStack, a / b); break;
            }
        }
    }

    *result = popLong(&operandStack); // Ergebnis abrufen
    return true; // Erfolgreich
} 

// Funktion: Eingabe trimmen und * zwischen Zahl und ( einfügen
void trimAndInsertMultiplication(char *input) {
    char tempBuffer[BUFFER_SIZE] = {0};
    int tempIndex = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (isdigit(input[i]) && (input[i + 1] == '(' || input[i + 1] == ' ' && input[i + 2] == '(')) {
            tempBuffer[tempIndex++] = input[i];
            tempBuffer[tempIndex++] = '*';
            if (input[i + 1] == ' ') i++; // Leerzeichen überspringen
        } else {
            tempBuffer[tempIndex++] = input[i];
        }
    }

    tempBuffer[tempIndex] = '\0';
    strcpy(input, tempBuffer); // Puffer kopieren
}

// Funktion: Eingabe verarbeiten und berechnen
void processInput(const char *input) {
    char formattedInput[BUFFER_SIZE];
    strcpy(formattedInput, input);
    trimAndInsertMultiplication(formattedInput); // Eingabe anpassen

    char postfix[BUFFER_SIZE] = {0};

    if (!toPostfix(formattedInput, postfix)) {
        Serial.println("Error"); // Fehlerhafte Eingabe
        return;
    }

    long result = 0;
    if (!evaluatePostfix(postfix, &result)) {
        Serial.println("Error"); // Fehler in der Berechnung
        return;
    }

    Serial.print(result); // Ergenis ausgeben
}

void setup() {
    Serial.begin(115200); // Serielle Kommunikation starten
}

void loop() {
    while (Serial.available() > 0) { // Überprüfen ob Daten verfügbar sind
        char c = Serial.read(); // Nächstets Zeichen lesen 

        if (c == '\r')
            continue; // Wagenrücklauf ignorieren

        if (c == '\n') { // Zeilenumbruch signalisiert das Ende der Eingabe
            inputBuffer[bufferIndex] = '\0'; // Eingabe null-terminieren 
            processInput(inputBuffer); // Eingabe verarbeiten
            bufferIndex = 0; // Puffer zurücksetzen 
            memset(inputBuffer, 0, BUFFER_SIZE); // Puffer leeren
        } else {
            if (bufferIndex < BUFFER_SIZE - 1) { // Eingabe speichern, falls Puffer nicht voll ist
                inputBuffer[bufferIndex++] = c;
            }
        }
    }
}
