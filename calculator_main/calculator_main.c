#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <windows.h>

#define BUFFER_SIZE 200 // Maximale Puffergröße

// Funktion zur Überprüfung auf Division durch Null
bool containsDivisionByZero(const char* input) {
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '/') { // Suche nach dem Divisionsoperator
            int j = i + 1;
            while (input[j] == ' ') j++; // Leerzeichen überspringen
            if (input[j] == '0') {
                return true; // Division durch Null gefunden
            }
        }
    }
    return false; // Keine Division durch null gefunden
}
// Funktion zur Überprüfung auf Dezimalzahlen
bool containsInvalidFormats(const char* input) {
    while (*input) {
        if (*input == '.' || *input == ',') {
            return true; // Dezimalzahl gefunden
        }
        input++;
    }
    return false; // Keine Dezimalzahl gefunden 
}

// Funktion zur Überprüfung auf unerlaubte Zeichen
bool containsInvalidCharacters(const char* input) {
    while (*input) {
        // Prüfen, ob das Zeichen außerhalb des gültigen ASCII-Bereich liegt
        if ((unsigned char)*input > 127) {
            return true; // Zeichen außerhalb des gültigen Bereichs gefunden (z.B. Sonderzeichen)
        }
        // Prüfen auf gültige Zeichen (Zahlen Operatoren, Klammern, Leerzeichen)
        if (!isdigit(*input) && !strchr("+-*/()", *input) && *input != ' ') {
            return true; // Unerlaubte Zeichen gefunden
        }
        input++;
    }
    return false; // Kein unerlaubtes Zeichen gefunden
}

// Funktion zur Validierung der Eingabe
bool validateInput(const char* input) {
    if (containsDivisionByZero(input)) { // Division durch Null gefunden
        printf("Fehler: Division durch Null ist nicht erlaubt.\n");
        return false;
    }
    if (containsInvalidFormats(input)) { // Dezimalzahlen gefunden
        printf("Fehler: Kommazahlen sind nicht erlaubt \n");
        return false;
    }
    if (containsInvalidCharacters(input)) { // Unerlaubet Zeichen gefunden
        printf("Fehler: Nicht erlaubte Zeichen in der Eingabe.\n");
        return false;
    }
    return true; // Eingabe gültig
}

// Funktion zur seriellen Kommunikation
void performSerialCommunication(const char* calculation, char* resultBuffer) {
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 }; // Struktur für serielle Port-Parameter
    COMMTIMEOUTS timeouts = { 0 }; // Struktur für Timeout-Einstellungen
    DWORD bytesWritten, bytesRead;

    // Seriellen Port öffnen
    hSerial = CreateFileA("\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        strncpy_s(resultBuffer, BUFFER_SIZE, "Error: Unable to open COM3", _TRUNCATE);
        return;
    }

    // Serielle Kommunikation konfigurieren
    dcbSerialParams.DCBlength = sizeof(DCB);
    GetCommState(hSerial, &dcbSerialParams); // Aktuelle Einstellung abrufen
    dcbSerialParams.BaudRate = CBR_115200; // Bautrate festlegen
    dcbSerialParams.ByteSize = 8; // Datenbits
    dcbSerialParams.StopBits = ONESTOPBIT; //  Stoppbit
    dcbSerialParams.Parity = NOPARITY; // Keine Parität
    SetCommState(hSerial, &dcbSerialParams); // Einstellungen übernehmen

    // Timeout-Werte festlegen
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 500;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    // Berechnung senden
    char bufferToSend[BUFFER_SIZE] = { 0 };
    strncpy_s(bufferToSend, BUFFER_SIZE, calculation, _TRUNCATE);
    strcat_s(bufferToSend, BUFFER_SIZE, "\n");
    if (!WriteFile(hSerial, bufferToSend, (DWORD)strlen(bufferToSend), &bytesWritten, NULL)) {
        CloseHandle(hSerial);
        strncpy_s(resultBuffer, BUFFER_SIZE, "Error: Failed to write to serial port", _TRUNCATE);
        return;
    }

    // Antwort empfangen
    Sleep(250); // 250 millisekunden Warten um Sicherzustellen das der Mikrocontroler antwortet
    char tempBuffer[BUFFER_SIZE] = { 0 };
    if (!ReadFile(hSerial, tempBuffer, BUFFER_SIZE - 1, &bytesRead, NULL)) {
        CloseHandle(hSerial);
        strncpy_s(resultBuffer, BUFFER_SIZE, "Error: Failed to read from serial port", _TRUNCATE);
        return;
    }
    tempBuffer[bytesRead] = '\0'; // Nullterminierung der empfangenen Daten
    strncpy_s(resultBuffer, BUFFER_SIZE, tempBuffer, _TRUNCATE);

    CloseHandle(hSerial); // Serriellen Port schließen
}

// Funktion zur Durchführung der seriellen Berechnung
void performSerialCalculation(const char* calculation) {
    char result[BUFFER_SIZE] = { 0 };
    performSerialCommunication(calculation, result); // Kommunikation mit dem Mikrocontroler 

    if (strlen(result) > 0) { // Prüfen ob eine Antwort empfangen wurde
        printf("Ergebnis vom Controler: %s\n", result); // Darstellen des Ergebins
    }
    else {
        printf("Fehler: Keine Antwort vom Controler erhalten.\n"); // Fehlermeldung fals kein Ergebnis empfangen wurde
    }
}
// Hauptfunktion 
int main() {
    char calculation[BUFFER_SIZE]; // Eingabepuffer 

    // Begrüßungsnachricht
    printf("Willkommen zum Taschenrechner für grundlegende algebraischer Berechnungen!\n");

    while (1) {
        printf("Geben Sie die Berechnung ein oder 'exit' zum Beenden:\n"); // Eingabeaufforderung 
        fgets(calculation, BUFFER_SIZE, stdin);

        // Zeilenumbruch entfernen
        calculation[strcspn(calculation, "\n")] = 0;

        // Programm beenden bei Eingabe von 'exit'
        if (strcmp(calculation, "exit") == 0) {
            printf("Programm beendet.\n");
            break;
        }

        // Eingabe validieren
        if (validateInput(calculation)) {
            performSerialCalculation(calculation);// Berechnung durchführen
        }
    }

    return 0; // Programm erfolgreich beendet
}
