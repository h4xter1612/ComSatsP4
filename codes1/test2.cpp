#include <iostream>
#include <RF24/RF24.h>
#include <unistd.h> // Para sleep y usleep
using namespace std;

RF24 radio(17, 0); // CE = GPIO17, CSN = CE0

const uint8_t address[6] = "00001"; // Dirección para pipes

int main() {
    cout << "================ INICIANDO PRUEBA nRF24L01+ ================\n";

    if (!radio.begin()) {
        cout << "❌ radio.begin() FALLÓ. Módulo no responde.\n";
        radio.printDetails();
        return 1;
    }

    cout << "✅ radio.begin() OK.\n";

    // Configuración de dirección de comunicación
    radio.openWritingPipe(address);      // Pipe para enviar
    radio.openReadingPipe(1, address);   // Pipe para recibir (necesario en algunos módulos)

    // Opcional: Nivel de potencia de transmisión
    radio.setPALevel(RF24_PA_LOW);

    // Estado inicial
    cout << "\n--- Estado INICIAL ---\n";
    radio.printDetails();

    // Modo recepción
    cout << "\n--- Cambiando a modo ESCUCHA (startListening) ---\n";
    radio.startListening();
    usleep(10000); // delay de 10ms
    radio.printDetails();

    // Modo transmisión
    cout << "\n--- Cambiando a modo ENVÍO (stopListening) ---\n";
    radio.stopListening();
    usleep(10000);
    radio.printDetails();

    cout << "================== FIN DIAGNÓSTICO ==================\n";
    return 0;
}

