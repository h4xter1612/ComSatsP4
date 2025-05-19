#include <iostream>
#include <RF24.h>

using namespace std;

#define PIN_CE 17   // Cambiado a 17
#define PIN_CSN 0   // Cambiado a 0

int main() {
    RF24 radio(PIN_CE, PIN_CSN);

    if (!radio.begin()) {
        cout << "ERROR: No se pudo inicializar el módulo NRF24." << endl;
        return 1;
    }

    cout << "Inicialización exitosa." << endl;

    radio.stopListening();  // Cambiar a modo de transmisión

    const char text[] = "Hola desde el transmisor!";

    // Intentar enviar el mensaje
    bool enviado = radio.write(&text, sizeof(text));

    if (enviado) {
        cout << "Mensaje enviado exitosamente." << endl;
    } else {
        cout << "Error al enviar el mensaje." << endl;
    }

    return 0;
}

