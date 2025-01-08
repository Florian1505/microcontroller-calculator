# Mikrocontroller-Taschenrechner

## Projektbeschreibung
Dieser Mikrocontroller-Taschenrechner ermöglicht es, algebraische Berechnungen wie `34 * 72 = 2448` über eine serielle Schnittstelle (RS232, UART) zwischen einem Personal Computer (PC) und einer Mikrocontroller-Platine durchzuführen. Das Projekt umfasst zwei Hauptteile:

1. **PC-Anwendung**: Sendet die Rechenanweisungen an den Mikrocontroller und zeigt das Ergebnis an.
2. **Mikrocontroller-Anwendung**: Führt die Berechnungen aus und sendet das Ergebnis zurück an den PC.

Das Projekt ist für ArduinoUno-Boards konzipiert, kann jedoch leicht an andere Boards mit UART-Unterstützung angepasst werden.

## Funktionen
- Serielle Kommunikation zwischen PC und Mikrocontroller.
- Unterstützung der vier Grundrechenarten: Addition, Subtraktion, Multiplikation und Division.
- Verarbeitung von algebraischen Ausdrücken im Format `Zahl1 Operator Zahl2`.
- Fehlerbehandlung bei:
  - Division durch Null.
  - Ungültigen Zeichen oder Formaten (z. B. Dezimalzahlen, Hochzahlen).
  - Überlauf bei zu großen Zahlen.
 
## Projektstruktur
```
📁 microcontroller_code
├── calculator_main
│   └── calculator_main.c  # PC-Anwendung
├── calculator_calc
│   └── calculator_calc.ino  # Mikrocontroller-Code
└── README.md  # Projektdokumentation
```
## Anforderungen
### Software
- Arduino IDE (für Mikrocontroller-Code)
- GCC-Compiler oder eine IDE wie Visual Studio Code für die PC-Anwendung

### Hardware
- Mikrocontroller-Board mit UART-Unterstützung (z.B. Arduino Uno)
- Verbindungskabel (z. B. USB oder UART)

## Installation
### Mikrocontroller-Code
1. Öffne die Datei `calculator_calc.ino` in der Arduino IDE.
2. Wähle das passende Board und den entsprechenden COM-Port aus.
3. Lade den Code auf den Mikrocontroller hoch.

### PC-Anwendung
1. Navigiere in das Verzeichnis `calculator_main`.
2. Kompiliere die Datei `calculator_main.c`:
   ```bash
   gcc -o calculator calculator_main.c -lws2_32
   ```
3. Führe die Anwendung aus:
   ```bash
   ./calculator
   ```
   
## Nutzung
1. Starte die PC-Anwendung.
2. Gib einen algebraischen Ausdruck wie `34 * 72` ein und drücke Enter.
3. Das Ergebnis wird angezeigt.
4. Gib `exit` ein, um das Programm zu beenden.

## Beispielausgabe
Willkommen zum Taschenrechner für grundlegende algebraischer Berechnungen!
Geben Sie die Berechnung ein oder 'exit' zum Beenden:
34 * 72
Ergebnis vom Arduino: 2448
```

## Fehlerbehandlung
- **Division durch Null**: Das Programm erkennt die Eingabe und gibt eine Fehlermeldung aus.
- **Ungültige Zeichen/Formate**: Eingaben mit Buchstaben, Sonderzeichen oder Dezimalzahlen werden zurückgewiesen.
- **Überlauf**: Wenn das Ergebnis oder eine Eingabe den zulässigen Bereich überschreitet, wird eine entsprechende Meldung angezeigt.
