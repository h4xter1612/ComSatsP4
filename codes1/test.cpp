#include <iostream>
#include <RF24/RF24.h>
using namespace std;

RF24 radio(17, 0); // CE = GPIO17, CSN = CE0

int main() {
    cout << "================ INICIANDO PRUEBA nRF24L01+ ================\n";

    if (!radio.begin()) {
        cout << "❌ radio.begin() FALLÓ. Módulo no responde.\n";
        radio.printDetails();
        return 1;
    }

    cout << "✅ radio.begin() OK.\n";

    // Estado inicial
    cout << "\n--- Estado INICIAL ---\n";
    radio.printDetails();

    // Modo recepción
    cout << "\n--- Cambiando a modo ESCUCHA (startListening) ---\n";
    radio.startListening();
    delay(10);
    radio.printDetails();

    // Modo transmisión
    cout << "\n--- Cambiando a modo ENVÍO (stopListening) ---\n";
    radio.stopListening();
    delay(10);
    radio.printDetails();

    cout << "================== FIN DIAGNÓSTICO ==================\n";
    return 0;
}
